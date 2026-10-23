/*
Copyright (C) 2018 Daniele Debernardi <drebrez@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <errno.h>
#include <linux/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

void usage(char *appname) {
  printf("Usage: %s [-h] MODE\n\n", appname);
  printf("Reboot the device to the MODE specified (e.g. recovery, "
         "bootloader)\n\n");
  printf("WARNING: the reboot is instantaneous\n\n");
  printf("optional arguments:\n");
  printf("  -h    Show this help message and exit\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage(argv[0]);
    exit(1);
  }

  int opt;
  while ((opt = getopt(argc, argv, "h")) != -1) {
    switch (opt) {
    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  }

  sync();

  int ret;
  ret = syscall(__NR_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
                LINUX_REBOOT_CMD_RESTART2, argv[1]);

  if (ret < 0) {
    printf("Error: %s\n", strerror(errno));
  }

  return ret;
}
