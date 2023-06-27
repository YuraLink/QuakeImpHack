#define _GNU_SOURCE
#include <link.h>
#include <string.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <SDL.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <stdbool.h>

#include "game_utils.h"

struct mmaped_elf_node
{
    struct mmaped_elf_node* next;
    void* address;
    char path[4096];
};

struct mmaped_elf_node* map_list_shared;

struct resolve_sym_info
{
    char name_shared_object[200];
    char symbol[200];
    void* address_symbol;
};

struct hook_relocation_info
{
    char name_shared_object[200];
    char symbol_name[200];
    void* prev_address;
    void* new_address;
};

centity_t* entity_list_ref = NULL;
cgs_t* client_game_stat_ref = NULL;
cg_t* client_game_ref = NULL;

SDL_GLContext original_context = NULL;
SDL_GLContext user_context = NULL;

void (*original_swap_buffer)(SDL_Window* window) = NULL;

void swap_window_hook(SDL_Window* window);

int hook_global_offset_table(struct dl_phdr_info *info, size_t size, void *data);

struct mmaped_elf_node* search_map_file(const char* const path, struct mmaped_elf_node* begin);

int add_file_to_map_list(const char* const path, struct mmaped_elf_node** begin, void** map_elf);

int resolve_symtab_symbol(struct dl_phdr_info *info, size_t size, void *data);

__attribute__((constructor)) void entry() {
    struct hook_relocation_info swap_buffer_hook = {"renderer_opengl2_x86_64.so", "SDL_GL_SwapWindow", NULL, swap_window_hook};
    struct resolve_sym_info client_game_score_sym = {"cgamex86_64.so", "cgs", NULL};
    struct resolve_sym_info client_game_entities_sym = {"cgamex86_64.so", "cg_entities", NULL};
    struct resolve_sym_info client_game_sym = {"cgamex86_64.so", "cg", NULL};

    dl_iterate_phdr(resolve_symtab_symbol, &client_game_score_sym);
    dl_iterate_phdr(resolve_symtab_symbol, &client_game_entities_sym);
    dl_iterate_phdr(resolve_symtab_symbol, &client_game_sym);
    dl_iterate_phdr(hook_global_offset_table, &swap_buffer_hook);

    client_game_stat_ref = client_game_score_sym.address_symbol;
    entity_list_ref = client_game_entities_sym.address_symbol;
    client_game_ref = client_game_sym.address_symbol;
    original_swap_buffer = SDL_GL_SwapWindow;

    assert(original_swap_buffer != NULL);
    assert(client_game_stat_ref != NULL);
    assert(entity_list_ref != NULL);
    assert(client_game_ref != NULL);
    assert(client_game_ref != NULL);
}

 void swap_window_hook(SDL_Window* window)
 {
    original_context = SDL_GL_GetCurrentContext();
    if (!user_context)
    {
        user_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, user_context);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 640, 480, 0, 1.0, -1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0, 0, 0, 1.0);
    }
    else
    {
        SDL_GL_MakeCurrent(window, user_context);
    }

    glBegin(GL_LINE_LOOP) ;
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.5, 0.0);
    glVertex3f(0.5, 0.5, 0.0);
    glVertex3f(0.5, 0.0, 0.0);
    glEnd() ;

    //SDL_GL_MakeCurrent(window, original_context);
    original_swap_buffer(window);
 }

char* get_path_exutable()
{
    static char path[4096];
    if (strlen(path) > 0)
        return path;
    if (readlink ("/proc/self/exe", path, sizeof(path)) != -1)
        return path;
}

struct mmaped_elf_node* search_map_file(const char* const path, struct mmaped_elf_node* begin)
{
    struct mmaped_elf_node* itr = begin;

    while (itr != NULL) {
        if (!strcmp(path, begin->path))
            return itr->address;
        itr = itr->next;
    }

    return NULL;
}

int add_file_to_map_list(const char* const path, struct mmaped_elf_node** begin, void** elf_map)
{
    int elf_file_fd = 0;
    int ret_err_val = 0;
    void* file_map_address = NULL;
    struct mmaped_elf_node* end;
    struct mmaped_elf_node* new_node;
    struct stat elf_file_stat;

    if (begin == NULL || path == NULL)
        return -1;

    if ((elf_file_fd = open(path, O_RDONLY)) < 0)
        return -2;

    if (fstat(elf_file_fd, &elf_file_stat) > 0)
        return -3;

    new_node = (struct mmaped_elf_node*) malloc(sizeof(struct mmaped_elf_node));
    if (new_node == NULL)
    {
        close(elf_file_fd);
        return -4;
    }

    file_map_address = mmap(0, elf_file_stat.st_size, PROT_READ, MAP_PRIVATE, elf_file_fd, 0);
    if (file_map_address == NULL)
    {
        free(new_node);
        close(elf_file_fd);
        return -5;
    }

    new_node->address = file_map_address;
    snprintf(new_node->path, 4096, "%s", path);
    *elf_map = file_map_address;

    if (*begin == NULL) {
        *begin = new_node;
        return 0;
    }

    end = *begin;
    while (end->next) end = end->next;
    end->next = new_node;
    return 0;
}

int resolve_symtab_symbol(struct dl_phdr_info *info, size_t size, void *data)
{
    void* maped_elf_file = NULL;
    const char* path_exutable = NULL;
    char* sh_strtab = NULL;
    char* strtab = NULL;
    Elf64_Ehdr* ehdr = NULL;
    Elf64_Shdr* shdr = NULL;
    Elf64_Shdr* shdr_symtab = NULL;
    Elf64_Shdr* shdr_strtab = NULL;
    Elf64_Shdr* shdr_shstrtab = NULL;
    Elf64_Sym* symtab = NULL;
    struct resolve_sym_info* sym_info = ( struct resolve_sym_info*) data;

    if (*info->dlpi_name == 0)
        path_exutable = get_path_exutable();
    else
        path_exutable = info->dlpi_name;

    if (strcmp(basename(path_exutable), sym_info->name_shared_object))
        return 0;

    maped_elf_file = search_map_file(path_exutable, map_list_shared);
    if (maped_elf_file == NULL)
        add_file_to_map_list(path_exutable, &map_list_shared,  &maped_elf_file);

    assert(maped_elf_file != NULL);

    ehdr = maped_elf_file;
    shdr = ehdr->e_shoff + maped_elf_file;
    shdr_shstrtab = ehdr->e_shstrndx + shdr;
    sh_strtab = shdr_shstrtab->sh_offset + maped_elf_file;

    shdr_symtab = shdr;
    while(shdr + ehdr->e_shnum > shdr_symtab && strcmp(sh_strtab + shdr_symtab->sh_name, ".symtab"))
        shdr_symtab++;

    shdr_strtab = shdr;
    while(shdr + ehdr->e_shnum > shdr_strtab && strcmp(sh_strtab + shdr_strtab->sh_name, ".strtab"))
        shdr_strtab++;

    assert(!strcmp(sh_strtab + shdr_symtab->sh_name, ".symtab"));
    assert(!strcmp(sh_strtab + shdr_strtab->sh_name, ".strtab"));

    symtab = shdr_symtab->sh_offset + maped_elf_file;
    strtab = shdr_strtab->sh_offset + maped_elf_file;
    for (int i = 0; i < shdr_symtab->sh_size / shdr_symtab->sh_entsize; i++)
    {
        if (!strcmp(strtab + symtab[i].st_name, sym_info->symbol)) {
            sym_info->address_symbol =  (void*) info->dlpi_addr + symtab[i].st_value;
            break;
        }
    }

    return 0;
}

int hook_global_offset_table(struct dl_phdr_info *info, size_t size, void *data)
{
    const Elf64_Phdr* begin_phdr = NULL;
    const Elf64_Phdr* dynamic_phdr = NULL;
    Elf64_Dyn* dynamic = NULL;
    Elf64_Sym* symtab = NULL;
    Elf64_Rel* rel_plt = NULL;
    Elf64_Rela* rel_addend_plt = NULL;
    Elf64_Addr rel_plt_ptr = 0;
    int plt_rel_size = 0;
    int reloc_struct_type = 0;
    int reloc_strcut_size = 0;
    const char* path_exutable = NULL;
    char* strtab = NULL;
    struct hook_relocation_info* hook_info = (struct hook_relocation_info*) data;

    if (*info->dlpi_name == 0)
        path_exutable = get_path_exutable();
    else
        path_exutable = info->dlpi_name;

    if (strcmp(basename(path_exutable), hook_info->name_shared_object))
        return 0;

    begin_phdr = info->dlpi_phdr;
    dynamic_phdr = begin_phdr;
    while (begin_phdr + info->dlpi_phnum > dynamic_phdr && dynamic_phdr->p_type != PT_DYNAMIC)
        dynamic_phdr++;

    assert(dynamic_phdr->p_type == PT_DYNAMIC);

    dynamic = (Elf64_Dyn*) (dynamic_phdr->p_vaddr + info->dlpi_addr);
    while (dynamic->d_tag != DT_NULL)
    {
        if (dynamic->d_tag == DT_JMPREL)
        rel_plt_ptr = dynamic->d_un.d_ptr;
        else if (dynamic->d_tag == DT_PLTREL)
            reloc_struct_type = dynamic->d_un.d_val;
        else if (dynamic->d_tag == DT_SYMTAB)
            symtab = (Elf64_Sym*) dynamic->d_un.d_ptr;
        else if (dynamic->d_tag == DT_STRTAB)
            strtab = (char*) dynamic->d_un.d_ptr;
        else if (dynamic->d_tag == DT_PLTRELSZ)
            plt_rel_size = dynamic->d_un.d_val;
        else if (dynamic->d_tag == DT_RELENT)
            reloc_strcut_size = dynamic->d_un.d_val;
        else if (dynamic->d_tag == DT_RELAENT)
            reloc_strcut_size = dynamic->d_un.d_val;

        dynamic++;
    }

    assert(plt_rel_size != 0);
    assert(reloc_struct_type != 0);
    assert(rel_plt_ptr != 0);
    assert(reloc_strcut_size != 0);
    assert(strtab != NULL);
    assert(symtab != NULL);

    if (reloc_struct_type == DT_RELA)
        rel_addend_plt = (Elf64_Rela*) rel_plt_ptr;
    else
        rel_plt = (Elf64_Rel*) rel_plt_ptr;

    for (int i = 0; i <  plt_rel_size / reloc_strcut_size; i++)
    {
        int index_sym = 0;
        int64_t* got_entry = NULL;

        if (reloc_struct_type == DT_RELA)
        {
            index_sym = ELF64_R_SYM(rel_addend_plt[i].r_info);
            got_entry = (void*) rel_addend_plt[i].r_offset + info->dlpi_addr;
        }
        else
        {
            index_sym = ELF64_R_SYM(rel_plt[i].r_info);
            got_entry = (void*) rel_addend_plt[i].r_offset + info->dlpi_addr;
        }

        Elf64_Sym symbol = symtab[index_sym];
        if (!strcmp(strtab + symbol.st_name, hook_info->symbol_name)) {
            hook_info->prev_address = (void*) *got_entry;
            *got_entry = (int64_t) hook_info->new_address;
        }
    }
}
