/**
 * @file vm.h
 * @brief Virtual memory management.
 * @details This file contains the virtual memory management.
 * @author ckoyama(koyamanX)
 */
#ifndef _VM_H
#define _VM_H

#include <alloc.h>
#include <riscv.h>

/**
 * @brief Enable paging with the given page table.
 * @details This function enables paging with the given page table.
 * @param[in] kpgtbl The page table to use.
 */
void kvmstart(pagetable_t kpgtbl);

/**
 * @brief Initialize the kernel page table.
 * @details This function initializes the kernel page table.
 * @return The kernel page table.
 */
pagetable_t kvminit(void);

/**
 * @brief Dump page table entry for givien virtual address.
 * @details This function dumps page table entry for givien virtual address.
 * @param[in] pgtbl The page table to use.
 * @param[in] va The virtual address to dump.
 */
void kvmdump(pagetable_t pgtbl, u64 va);

/**
 * @brief Map a virtual address to a physical address.
 * @details This function maps a virtual address to a physical address.
 * @param[in] pgtbl The page table to use.
 * @param[in] va The virtual address to map.
 * @param[in] pa The physical address to map.
 * @param[in] sz The size of the mapping.
 * @param[in] perm The permission of the mapping.
 */
void kvmmap(pagetable_t pgtbl, u64 va, u64 pa, u64 sz, u64 perm);

/**
 * @brief Walk the page table.
 * @details This function walks the page table.
 * @param[in] pgtbl The page table to use.
 * @param[in] va The virtual address to walk.
 * @return The page table entry.
 */
pte_t *kvmwalk(pagetable_t pgtbl, u64 va);

/**
 * @brief Allocate a page table entry.
 * @details This function allocates a page table entry for the given virtual
 * address.
 * @param[in] pgtbl The page table to use.
 * @param[in] va The virtual address to allocate.
 * @return The page table entry.
 */
pte_t *kvmalloc(pagetable_t pgtbl, u64 va);

/**
 * @brief Initialize the kernel memory allocator.
 * @details This function initializes the kernel memory allocator.
 */
void kmeminit(void);

/**
 * @brief Allocate a pages with the given order.
 * @details This function allocates a pages with the given order.
 * @param[in] order The order of the pages.
 * @return The pointer to the allocated pages.
 */
void *alloc_page(void);

// Delegate all memory from start to end to user space procmgr.
u64 delegate_memory(pagetable_t pgtbl, u64 end);

/**
 * @brief Convert a virtual address to a physical address.
 * @details This function converts a virtual address to a physical address using
 * the given page table.
 * @param[in] pgtbl The page table to use.
 * @param[in] va The virtual address to convert.
 * @return The physical address.
 */
u64 va2pa(pagetable_t pgtbl, u64 va);

/**
 * @brief copy len byte from user space to kernel space.
 * @details This function copies len byte from user space to kernel space.
 * @param[in] uaddr The user space address.
 * @param[in] kaddr The kernel space address.
 * @param[in] len The length of the copy.
 * @return success(0) or failure(-1).
 */
int copyin(const void *uaddr, void *kaddr, size_t len);

/**
 * @brief copy len byte from kernel space to user space.
 * @details This function copies len byte from kernel space to user space.
 * @param[in] kaddr The kernel space address.
 * @param[in] uaddr The user space address.
 * @param[in] len The length of the copy.
 * @return success(0) or failure(-1).
 */
int copyout(const void *kaddr, void *uaddr, size_t len);

/**
 * @brief copy string from user space to kernel space for len byte.
 * @details This function copies string from user space to kernel space for len
 * byte.
 * @param[in] uaddr The user space address.
 * @param[in] kaddr The kernel space address.
 * @param[in] len The length of the copy.
 * @param[out] done The length of the copied string.
 * @return success(0) or failure(-1).
 */
int copyinstr(const void *uaddr, void *kaddr, size_t len, size_t *done);

#endif  // _VM_H
