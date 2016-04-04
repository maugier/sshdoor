# SSHDoor

An `LD_PRELOAD` wrapper to hide a network service (typically SSH) behind another.

## Usage

Edit sshdoor.c and adjust according to your needs:

 * Change `trigger` to whatever secret value in the beginning of the stream should trigger the alternate service. The default
   of `SSH-2.0-` is fine for SSH. You want this to be as short as possible

 * Change `program` to whichever service you wish to run. Note that for ssh, the server must be run as root. You can use
   `{ "sshd", "-i" }` for minimal overhead. Another easy alternative is to use `{"nc", "-q0", "localhost", "22"}` to defer
   to an already-running sshd.

Compile the wrapper:

   make

Edit your service scripts to set `LD_PRELOAD=/path/to/sshdoor.so`
