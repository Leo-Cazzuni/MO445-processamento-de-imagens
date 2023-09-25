------------------------------------------------------------------------------------------------------------------------
-- True dual port RAM.
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
------------------------------------------------------------------------------------------------------------------------
-- RAM with two read / write ports (ports A and B)
--TRUE DUAL PORT RAM uses two read ports and two write ports operating in pairs of RW, each pair share the
-- same address. This memory can only be implemented using blockrams, in any size and bus width.
--
--USE:
-- - Total number of memory positions: 2**ram_size_c
-- - Total memory size (bits): 2**ram_size_c * port_size_c
-- - Memory types:
--     - For blockram,  use the string "block"
--     - For distributted, use the string "distributed"
--     - For FLIP-FLOP,  use the string "flip-flop"
--
-- Notes:
-- - Blockram will always be of synchronous output.
-- - Memory behavior may change due to parameter selection.
-- - Memory clock and total possible size is (and will always be) family dependent.
------------------------------------------------------------------------------------------------------------------------
entity true_dualport_ram is
    Generic (
        addr_size_c : integer := 8; --Address bus size constant
        port_size_c : integer := 8  --Data bus size constant
    );
    Port (
        rsta_i   : in  std_logic;                                   -- Port A reset
        clka_i   : in  std_logic;                                   -- Port A clock
        wrena_i  : in  std_logic;                                   -- Port A write enable
        rdena_i  : in  std_logic;                                   -- Port A read enable
        ena_i    : in  std_logic;                                   -- Port A chip select
        dataa_i  : in  std_logic_vector(port_size_c-1 downto 0);    -- Port A input data bus
        dataa_o  : out std_logic_vector(port_size_c-1 downto 0);    -- Port A output data bus
        addra_i  : in  std_logic_vector(addr_size_c-1 downto 0);    -- Port A address bus
        rstb_i   : in  std_logic;                                   -- Port B reset
        clkb_i   : in  std_logic;                                   -- Port B clock
        wrenB_i  : in  std_logic;                                   -- Port B write enable
        rdenB_i  : in  std_logic;                                   -- Port B read enable
        enb_i    : in  std_logic;                                   -- Port B chip select
        datab_i  : in  std_logic_vector(port_size_c-1 downto 0);    -- Port B input data bus
        datab_o  : out std_logic_vector(port_size_c-1 downto 0);    -- Port B output data bus
        addrb_i  : in  std_logic_vector(addr_size_c-1 downto 0)     -- Port B address bus
    );
end true_dualport_ram;

architecture rtl of true_dualport_ram is

    type   ram_t is array (2**addr_size_c-1 downto 0) of std_logic_vector(port_size_c-1 downto 0);
    shared variable ram_s : ram_t := (others => (others => '0'));
    signal dataa_o_s : std_logic_vector(port_size_c-1 downto 0);
    signal datab_o_s : std_logic_vector(port_size_c-1 downto 0);

begin

------------------------------------------------------------------------------------------------------------------------
--INPUT
------------------------------------------------------------------------------------------------------------------------
    --Input process for port A
    input_brama_p : process(clka_i)
        begin
            if clka_i'event and clka_i = '1' then
                if ena_i = '1' then
                    if wrena_i = '1' then
                        ram_s(conv_integer(addra_i)) := dataa_i;
                    end if;
                    dataa_o <= ram_s(conv_integer(addra_i));
                end if;
            end if;
    end process;

    --Input process for port B
    input_bramb_p : process(clkb_i)
        begin
            if clkb_i'event and clkb_i = '1' then
                if enb_i = '1' then
                    if wrenb_i = '1' then
                        ram_s(conv_integer(addrb_i)) := datab_i;
                    end if;
                    datab_o <= ram_s(conv_integer(addrb_i));
                end if;
            end if;
    end process;

end rtl;
