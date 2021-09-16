#!/bin/bash
#
# By: Brody Rethy
# Website: https://rethy.xyz
# GitHub: https://github.com/rethyxyz
#
# Name: rb.sh
#
# Summary:
# A simple script to imitate the Windows recycle bin system on Unix-like
# systems.

#
# TODOs
#
# TODO: Implement a form of file compression.
#
# TODO: Check if $TRASH_BIN variable in argument list. if so, quit, and ask the
# user why in the world they are trying to move the trash bin to the trash bin.

#
# Variables
#
# This can be changed to anything.
TRASH_BIN="$HOME/.trash"

#
# Functions
#
display_help() {
    printf "%s [ITEM] [ITEM 2] [ITEM 3] ...\n\n" "$0"
    printf "List the ITEM(s) you want to remove.\n"
}

handle_item_exists() {
    ITEM_PREFIX=0
    NEW_ITEM="$ITEM"

    while [ -e "$TRASH_BIN/$NEW_ITEM" ]; do
        ITEM_PREFIX=$((ITEM_PREFIX+1))
        NEW_ITEM="${ITEM_PREFIX}_${ITEM}"
    done

    mv "$ITEM" "$NEW_ITEM" \
    || sudo mv "$ITEM" "$NEW_ITEM"

    ITEM="$NEW_ITEM"
}

#
# Main
#
# If no arguments given, alert and exit.
[ $# -eq 0 ] && { printf "No filename(s) given\n"; display_help; exit 1; }

# If arg matches, call display_help().
[ "$*" = "--help" ] || [ "$*" = "-h" ] && { display_help; exit 0; }

# If $TRASH_BIN directory doesn't exists, alert and create it.
[ ! -d "$TRASH_BIN" ] && { \
    mkdir -p "$TRASH_BIN"
    printf "Created trash bin at \"%s\".\n" "$TRASH_BIN"
}

# MAIN LOOP.
for ITEM in "$@"; do
    # If item doesn't exist, alert and pass.
    [ ! -e "$ITEM" ] && {
        printf "Item \"%s\" does not exist.\n" "$ITEM"
        continue
    }

    # If link, alert and remove.
    if [ -L "$ITEM" ]; then
        {
            rm -rf "$ITEM" \
            && printf "Removed link \"%s\"\n." "$ITEM"
        } || {
            sudo rm -rf "$ITEM" \
            && printf "Removed link \"%s\" using sudo.\n" "$ITEM"
        }
    # If ITEM empty, alert and remove.
    elif [ ! -s "$ITEM" ]; then
        {
            rm -rf "$ITEM" \
            && printf "Removed empty file \"%s\".\n" "$ITEM"
        } || {
            sudo rm -rf "$ITEM" \
            && printf "Removed empty file \"%s\" using sudo.\n" "$ITEM"
        }
    else
        # If ITEM exists in recycle bin, rename until no match found.
        [ -e "$TRASH_BIN/$ITEM" ] && handle_item_exists

        # Move item file to trash.
        {
            mv "$ITEM" "$TRASH_BIN/" \
            && printf "Item \"%s\" to \"%s\"\n" "$ITEM" "$TRASH_BIN"
        } || {
            sudo mv "$ITEM" "$TRASH_BIN/" \
            && printf "Item \"%s\" to \"%s\" using sudo\n" "$ITEM" "$TRASH_BIN"
        }
    fi
done

exit 0
