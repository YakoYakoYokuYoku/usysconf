/*
 * This file is part of usysconf.
 *
 * Copyright © 2017 Solus Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cli.h"
#include "context.h"

int usc_cli_run_triggers(__usc_unused__ int argc, __usc_unused__ char **argv)
{
        autofree(UscContext) *context = NULL;

        if (geteuid() != 0) {
                fprintf(stderr, "You must be root to run triggers\n");
                return EXIT_FAILURE;
        }

        context = usc_context_new();
        if (!context) {
                fputs("Cannot create context!\n", stderr);
                return EXIT_FAILURE;
        }

        return usc_context_run_triggers(context, NULL) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 expandtab:
 * :indentSize=8:tabSize=8:noTabs=true:
 */
