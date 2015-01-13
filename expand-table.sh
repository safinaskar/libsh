#!/bin/bash

set -e

[ $# != 5 ] && echo "Usage: ${0##*/} WIDTH..." >&2 && exit 1

while IFS="" read -r LINE; do
	read -r CODE <<< "${LINE%%#*}"

	[ "$CODE" = "" ] && printf '%s\n' "$LINE" && continue

	if [ "$(grep -o '|' <<< "$CODE")" != $'|\n|\n|\n|\n|' ]; then
		echo "${0##*/}: not a |||||, line:" >&2
		printf '%s\n' "$LINE" >&2
		exit 1
	fi

	IFS="|" read -r COL1 COL2 COL3 COL4 COL5 COL6 <<< "$LINE"

	read -r COL1 <<< "$COL1"
	read -r COL2 <<< "$COL2"
	read -r COL3 <<< "$COL3"
	read -r COL4 <<< "$COL4"
	read -r COL5 <<< "$COL5"
	read -r COL6 <<< "$COL6"

	[ -n "$LEN" ] && echo "${#COL1} ${#COL2} ${#COL3} ${#COL4} ${#COL5} ${#COL6}" >&2

	( # Because we "shift"
		for COL in "$COL1" "$COL2" "$COL3" "$COL4" "$COL5"; do
			while [ "${#COL}" -lt "$1" ]; do
				COL="$COL "
			done

			printf '%s' "$COL | "

			shift
		done

		printf '%s\n' "$COL6"
	)

	if [ "$COL4" = custom ]; then
		while :; do
			! IFS="" read -r CUSTOM && echo "${0##*/}: cannot read custom line" >&2 && exit 1

			[ "$CUSTOM" = "}" ] && break

			printf '%s\n' "$CUSTOM"
		done

		printf '}\n'
	fi
done
