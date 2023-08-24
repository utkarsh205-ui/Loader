#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;
typedef int ("functocallfib")();

/*
 * release memory and other cleanups
 */
void loader_cleanup()
{
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
