#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
typedef int("functocallfib")();

/*
 * release memory and other cleanups
 */
void loader_cleanup()
{
  munmap(phdr, sizeof(Elf32_Phdr) * ehdr->e_phnum);
  munmap(ehdr, sizeof(Elf32_Ehdr));
  close(fd);
}

/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char **exe)
{
  fd = open(argv[1], O_RDONLY);

  int result = _start();
  printf("User _start return value = %d\n", result);
}

void load_and_run_elf(char **exe)
{
  fd = open(exe[1], O_RDONLY);
  if (fd == -1)
  {
    perror("open");
    exit(EXIT_FAILURE);
  }
  ehdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));
  ssize_t sz = read(fd, ehdr, sizeof(Elf32_Ehdr));
  if (!ehdr)
  {
    perror("Failed to open the file");
    return;
  }

  phdr = (Elf32_Phdr *)malloc(sizeof(Elf32_Phdr) * ehdr->e_phnum);
  if (!phdr)
  {
    perror("Failed to make the phdr table");
    return;
  }

  ssize_t sz2 = read(fd, phdr, sizeof(Elf32_Phdr) * ehdr->e_phnum);
  if (sz2 != sizeof(Elf32_Phdr) * ehdr->e_phnum)
  {
    perror("not able to allocate");
    free(phdr);
    close(fd);
    exit(EXIT_FAILURE);
  }

  int iter = ehdr->e_phnum;

  for (int i = 0; i < iter; i++)
  {
    if (phdr[i].p_type == PT_LOAD)
    {
      // mmemory mmap ke through
      void *address = mmap(NULL, phdr[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
      if (address == MAP_FAILED)
      {
        perror("mmap not been made");
        exit(EXIT_FAILURE);
      }
      size_t start = ehdr->e_entry - phdr[i].p_vaddr;

      // niche wala function is to load the memory p_filesz
      pread(fd, address, phdr[i].p_filesz, phdr[i].p_offset);
      void *entry = address + start;

      if (entry >= address && entry < address + phdr[i].p_memsz)
      {
        functocallfib _start = (functocallfib)entry;

        int result = _start();
        printf("result is %d\n", result);
        break;
      }
      munmap(address, phdr[i].p_memsz);
    }
  }
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: %s <ELF Executable> \n", argv[0]);
    exit(1);
  }
  int f1 = open(argv[1], O_RDONLY);
  if (f1 == -1)
  {
    perror("not able to open");
    exit(1);
  }

  // to check mistakes the loader if possible
  Elf32_Ehdr ehdr1;
  ssize_t sz = read(f1, &ehdr1, sizeof(Elf32_Ehdr));
  if (sz != sizeof(Elf32_Ehdr))
  {
    perror("not be possble to read");
    close(f1);
    exit(1);
  }

  Elf32_Phdr phdr1;
  ssize_t sz2 = read(f1, &phdr1, sizeof(Elf32_Phdr));
  if (sz2 != sizeof(Elf32_Phdr))
  {
    perror("read");
    close(f1);
    exit(1);
  }

  close(f1);

  load_and_run_elf(argv[1]);

  loader_cleanup();
  return 0;
}
