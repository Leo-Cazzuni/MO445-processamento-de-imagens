------------------------------------------------------------------------------------------------------------------------
-- Generic bit wide RAM
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
--Base library
library base_lib;
use base_lib.base_lib_pkg.all;
------------------------------------------------------------------------------------------------------------------------
-- RAM with 1 write / read port (port A) and one read-only port (port B)
-- Bit wide generic RAM, based on generic_ram, implemented to have a width in std_logic port, avoiding
-- the memory bus to be used with arrays with widths of "(0 downto 0)".
--
--USE:
-- - Total number of memory positions: 2**ram_size_c
-- - Total memory size (bits): 2**ram_size_c * port_size_c
-- - Memory types:
--     - For blockram,  use the string "block"
--     - For distributted, use the string "distributed"
--     - For FLIP-FLOP,  use the string "flip-flop"
-- Notes:
-- - Blockram will always be of synchronous output.
-- - Memory behavior may change due to parameter selection.
-- - Memory clock and total possible size is (and will always be) family dependent.
------------------------------------------------------------------------------------------------------------------------
entity bit_ram is
    Generic (
        addr_size_c : integer := 8;
        output_sync : boolean := false;
        ram_type    : string  := "distributed"
    );
    Port (
        rsta_i   : in  std_logic;
        clka_i   : in  std_logic;
        wrena_i  : in  std_logic;
        ena_i    : in  std_logic;
        dataa_i  : in  std_logic;
        dataa_o  : out std_logic;
        addra_i  : in  std_logic_vector(addr_size_c-1 downto 0);
        rstb_i   : in  std_logic;
        clkb_i   : in  std_logic;
        enb_i    : in  std_logic;
        datab_o  : out std_logic;
        addrb_i  : in  std_logic_vector(addr_size_c-1 downto 0)
    );
end bit_ram;

architecture rtl of bit_ram is

    signal dataa_i_s : std_logic_vector(0 downto 0);
    signal dataa_o_s : std_logic_vector(0 downto 0);
    signal datab_o_s : std_logic_vector(0 downto 0);

begin

    --conversion.
    dataa_i_s(0)    <= dataa_i;
    dataa_o         <= dataa_o_s(0);
    datab_o         <= datab_o_s(0);

    ram_u : generic_ram
        generic map(
            addr_size_c => addr_size_c,
            port_size_c => 1,
            output_sync => output_sync,
            ram_type    => ram_type
        )
        port map (
            rsta_i   => rsta_i,
            clka_i   => clka_i,
            wrena_i  => wrena_i,
            ena_i    => ena_i,
            dataa_i  => dataa_i_s,
            dataa_o  => dataa_o_s,
            addra_i  => addra_i,
            rstb_i   => rstb_i,
            clkb_i   => clkb_i,
            enb_i    => enb_i,
            datab_o  => datab_o_s,
            addrb_i  => addrb_i
        );

end rtl;
