#!/bin/bash
#
# By: Brody Rethy
# Website: https://rethy.xyz
#
# Name: rb.sh
#
# Summary:
# A simple script to imitate the Windows recycle bin on Unix.
#

#+++++++#
# TODOs #
#+++++++#

# TODO: Implement a form of file compression.

#+++++++++++#
# Variables #
#+++++++++++#

# This can be changed to anything.
TRASH_BIN="$HOME/.Trash"

#+++++++++++#
# Functions #
#+++++++++++#

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

    mv "$ITEM" "$NEW_ITEM" || sudo mv "$ITEM" "$NEW_ITEM"

    ITEM="$NEW_ITEM"
}

#++++++#
# Main #
#++++++#

# If not args given, printf and exit.
[ $# -eq 0 ] && { printf "No filename(s) given\n"; display_help; exit 1; }

# Else if an arg matches, call display_help.
if [ "$*" = "--help" ] || [ "$*" = "-h" ]; then display_help; exit 0; fi

# If $TRASH_BIN directory doesn't exists, create it.
[ ! -d "$TRASH_BIN" ] && mkdir -p "$TRASH_BIN"

# Main program loop. Loops through arguments given.
for ITEM in "$@"; do
    # Pass on current array index if item doesn't exist.
    [ ! -e "$ITEM" ] && {
        printf "Item \"%s\" does not exist.\n" "$ITEM";
        continue;
    }

    # If link, remove.
    if [ -L "$ITEM" ]; then
        {
            rm -rf "$ITEM" && printf "Removed link \"%s\"\n." "$ITEM"
        } || {
            sudo rm -rf "$ITEM" \
            && printf "Removed link \"%s\" using sudo.\n" "$ITEM"
        }
    # If ITEM empty, remove.
    elif [ ! -s "$ITEM" ]; then
        {
            rm -rf "$ITEM" && printf "Removed empty file \"%s\".\n" "$ITEM"
        } || {
            sudo rm -rf "$ITEM" \
            && printf "Removed empty file \"%s\" using sudo.\n" "$ITEM"
        }
    else
        # If ITEM exists in recycle bin, rename until no match.
        [ -e "$TRASH_BIN/$ITEM" ] && handle_item_exists

        # Move file to trash.
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
