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

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "files.h"

/* Implemented elsewhere in the codebase */
extern UscHandler usc_handler_icon_cache;

/* Table of supported handlers */
static const UscHandler *usc_handlers[] = { &usc_handler_icon_cache };

/**
 * Will be used to actually check if the path is updated, by consulting
 * the mtime of the tree against our known mtime.
 *
 * For now, let's cheat, because live stream.
 */
static bool usc_path_updated(__usc_unused__ const char *path)
{
        time_t sys_updated = 0;
        if (!usc_file_mtime(path, &sys_updated)) {
                return true;
        }
        /* TODO: Something useful with the mtime. */
        fprintf(stderr, "%s mtime: %ld\n", path, sys_updated);
        return true;
}

static void usc_handle_one(const UscHandler *handler, UscContext *context)
{
        UscHandlerStatus status = USC_HANDLER_MIN;
        const char *root = NULL;

        root = usc_context_get_prefix(context);

        for (size_t i = 0; i < handler->n_paths; i++) {
                const char *path = NULL;
                autofree(char) *full_path = NULL;
                autofree(char) *resolved_path = NULL;

                path = handler->paths[i];

                if (asprintf(&full_path, "%s/%s", root, path) < 0) {
                        fputs("OOM\n", stderr);
                        abort();
                }

                resolved_path = realpath(full_path, NULL);
                if (!resolved_path) {
                        /* This guy doesn't exist */
                        continue;
                }

                /* Do we need to handle this dude ? */
                if (!usc_path_updated(resolved_path)) {
                        continue;
                }

                status = handler->exec(context, path, resolved_path);
                switch (status) {
                case USC_HANDLER_FAIL:
                        /* Update record */
                        fputs("Failed\n", stderr);
                        break;
                case USC_HANDLER_SUCCESS:
                        /* Update record */
                        fputs("Success\n", stderr);
                        break;
                case USC_HANDLER_SKIP:
                        /* Can't run right now, so don't update mtimes */
                        fprintf(stderr, "Skipping: %s\n", handler->name);
                        break;
                default:
                        /* You done goofed */
                        fprintf(stderr, "Invalid return! %s\n", handler->name);
                        break;
                }
        }
}

int main(__usc_unused__ int argc, __usc_unused__ char **argv)
{
        autofree(UscContext) *context = NULL;

        context = usc_context_new("/");
        if (!context) {
                fputs("Cannot continue without valid UscContext\n", stderr);
                return EXIT_FAILURE;
        }

        if (usc_context_has_flag(context, USC_FLAGS_CHROOTED)) {
                fputs("Chrooted!\n", stdout);
        } else {
                fputs("Not chrooted\n", stdout);
        }

        /* Just test the main loop iteration jank for now */
        for (size_t i = 0; i < ARRAY_SIZE(usc_handlers); i++) {
                usc_handle_one(usc_handlers[i], context);
        }

        return EXIT_SUCCESS;
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
