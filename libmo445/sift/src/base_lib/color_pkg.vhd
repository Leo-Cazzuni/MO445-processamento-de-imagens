-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
--Standard libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
--Basic library
library base_lib;
use base_lib.base_lib_pkg.all;

-------------------------
-- Package declaration --
-------------------------
package color_pkg is

    constant COLOR_BLACK_C   : string := esc & "[0;30m";  --' # Black - Regular
    constant COLOR_RED_C     : string := esc & "[0;31m";  --' # Red
    constant COLOR_GREEN_C   : string := esc & "[0;32m";  --' # Green
    constant COLOR_YELLOW_C  : string := esc & "[0;33m";  --' # Yellow
    constant COLOR_BLUE_C    : string := esc & "[0;34m";  --' # Blue
    constant COLOR_PURPLE_C  : string := esc & "[0;35m";  --' # Purple
    constant COLOR_CYAN_C    : string := esc & "[0;36m";  --' # Cyan
    constant COLOR_WHITE_C   : string := esc & "[0;37m";  --' # White
    constant COLOR_BOLD_C    : string := esc & "[1;30m";  --' # Black - Bold
    constant COLOR_RESET_C   : string := esc & "[1;0m";   --' # Text reset

    constant COLOR_INFO_C     : string := COLOR_GREEN_C; 
    constant COLOR_WARNING_C  : string := COLOR_YELLOW_C; 
    constant COLOR_ERROR_C    : string := COLOR_RED_C; 

end color_pkg;

package body color_pkg is

end color_pkg;

