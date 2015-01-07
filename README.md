# LZW
C implementation of the Lempel Ziv Welch compression algorithm for CPSC 323

Write file compression and decompression filters:

      encode [ [-m MAXBITS] | [-e] | [-d DELAY] | [-p] ]*

      decode

encode reads a stream of characters from the standard input, compresses it
using the Lempel-Ziv-Welch algorithm, and writes the stream of codes to the
standard output as a stream of bits packed into 8-bit bytes.  decode reads
from the standard input a byte stream written by encode, decompresses the
stream of codes, and writes the stream of characters to the standard output.

encode and decode are a single program (i.e., they are hard links to the same
inode) whose behavior is determined by the name by which it is invoked (i.e.,
the 0-th command-line argument).  The options "-m MAXBITS", "-e", "-d DELAY",
and "-p" may appear in any order and any number of times; the last occurrence
supersedes any earlier ones.

encode writes codes using the number of bits required to specify valid codes
at the time (e.g., 9 bits when there are 512 valid codes, but 10 bits once the
next code is assigned), up to a maximum of 12 bits (or MAXBITS if the -m flag
is specified).

Normally the string table is initialized to contain all one-character strings.
However, with the escape option -e, the table is initialized with only the
empty string; and when a character appears in the input for the first time,
it is sent as a special escape code followed by the character (using 8 bits,
where 8 = CHAR_BIT in <limits.h>).

Most strings that are added to the table are never used.  With the delay option
-d DELAY, where DELAY is a positive integer, encode does not assign a code to a
string found during greedy parsing for the first DELAY times that it appears.
This entails keeping track of the strings that have appeared but have not been
assigned codes and the number of times that they have been seen.  These strings
are kept in the same data structure as those that have been assigned codes.
(Note that omitting the delay option is equivalent to setting DELAY = 0.)

When all possible codes have been assigned, the default is to stop assigning
codes.  However, with the pruning option -p, encode instead prunes the data
structure that contains the strings that have been assigned codes (and if the
delay option was specified, the strings that have appeared but have not been
assigned codes).  The pruning is performed in a manner that is equivalent to
the following:

* All existing assignments of codes to strings are revoked.

* The #appearances field for every string is halved, with any remainder
  discarded.  If #appearances is now 0, the string is deleted from the data
  structure.

* Unless the -e flag is specified, every one-character string is added to the
  data structure (if necessary) and assigned a code.

* Every other string whose #appearances is greater than DELAY (0 if the delay
  option was not specified) is assigned a code.

Note that this entails keeping track of the number of times that all strings
have been seen.

Point to Ponder:  Why will the pruned data structure contain every nonempty
prefix of every string in the data structure.  Why is this important?

Encode uses a (character) trie to make string table lookup efficient.  A
straight-forward implementation would keep a fixed-length array of pointers in
each node, one for each possible child, and set the pointer to NULL if the
child were empty.  However, on most inputs very few nodes have more than a few
children and most have none, so this approach would waste a LOT of storage.
Thus encode only stores the non-NULL child pointers in each node (e.g., in a
variable-length, array of (char, ptr) pairs, ordered by increasing values of
char) and uses binary search in that array to retrieve them.  (Linear search
would be too slow.)
