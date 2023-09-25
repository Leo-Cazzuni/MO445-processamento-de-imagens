------------------------------------------------------------------------------------------------------------------------
-- generic RAM.
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
-- base li
library base_lib;
    use base_lib.base_lib_pkg.all;

------------------------------------------------------------------------------------------------------------------------
-- RAM with 1 write / read port (port A) and one read-only port (port B)
--This memory implements usual type of memory over FPGA. Family and manufacturer independent.
--
--USE:
-- - Total number of memory positions: 2**ram_size_c
-- - Total memory size (bits): 2**ram_size_c * PORT_SIZE_C
-- - Memory types:
--     - For blockram,  use the string "block"
--     - For distributed, use the string "distributed"
--     - For FLIP-FLOP,  use the string "flip-flop"
--
-- Notes:
-- - Blockram will always be of synchronous output.
-- - Memory behavior may change due to parameter selection.
-- - Memory clock and total possible size is (and will always be) family dependent.
------------------------------------------------------------------------------------------------------------------------
entity generic_ram_sv is
    generic (
        ADDR_SIZE_C : integer := 8;      --Address bus size
        PORT_SIZE_C : integer := 8;      --Data bus size
        OUTPUT_SYNC : boolean := true;   --Output mode selection
        RAM_TYPE    : string  := "block" --Memory type selection
    );
    port (
        rsta_i   : in  std_logic;                                   -- port A reset
        clka_i   : in  std_logic;                                   -- port A clock
        wrena_i  : in  std_logic;                                   -- port A write enable
        ena_i    : in  std_logic;                                   -- port A enable
        dataa_i  : in  std_logic_vector(PORT_SIZE_C-1 downto 0);    -- port A input data bus
        dataa_o  : out std_logic_vector(PORT_SIZE_C-1 downto 0);    -- port A output data bus
        addra_i  : in  std_logic_vector(ADDR_SIZE_C-1 downto 0);    -- port A address bus
        rstb_i   : in  std_logic;                                   -- port B reset
        clkb_i   : in  std_logic;                                   -- port B clock
        enb_i    : in  std_logic;                                   -- port B enable
        datab_o  : out std_logic_vector(PORT_SIZE_C-1 downto 0);    -- port B output data bus
        addrb_i  : in  std_logic_vector(ADDR_SIZE_C-1 downto 0)     -- port B address bus
    );
end generic_ram_sv;

architecture rtl of generic_ram_sv is

    type   ram_t is array (2**ADDR_SIZE_C-1 downto 0) of std_logic_vector(PORT_SIZE_C-1 downto 0);
    shared variable ram_sv : ram_t := (others => (others => '0'));

    signal addra_s         : std_logic_vector(ADDR_SIZE_C-1 downto 0);    -- port A address bus
    signal wrena_s         : std_logic;
    signal ena_s           : std_logic;

    signal dataa_i_s       : std_logic_vector(PORT_SIZE_C-1 downto 0);    -- port B output data bus

    attribute ram_style : string;
    attribute ram_style of ram_sv : variable is RAM_TYPE;

begin

    port_a_delay : process(clka_i)
    begin
        if clka_i'event and clka_i = '1' then
            dataa_i_s<= dataa_i;
            addra_s  <= addra_i;
            wrena_s  <= wrena_i;
            ena_s    <= ena_i;
        end if;
    end process;

    assert RAM_TYPE = "flip-flop" or RAM_TYPE = "block" or RAM_TYPE = "distributed"
        report "Value '" & RAM_TYPE & "' for generic RAM_TYPE is invalid. Should be 'block', 'distributed' or 'flip-flop'"
        severity failure;

------------------------------------------------------------------------------------------------------------------------
--INPUT
------------------------------------------------------------------------------------------------------------------------
    --Data input, flip-flop mode.
    --There is full memory reset.
    in_ffd_gen : if RAM_TYPE = "flip-flop" generate
        input_ffd_p : process(clka_i, rsta_i)
            begin
                if rsta_i = '1' then
                    ram_sv := (others=>(others=>'0'));
                elsif clka_i'event and clka_i = '1' then
                    if wrena_s = '1' and ena_s = '1' then
                        ram_sv(conv_integer(addra_s)) := dataa_i_s;
                    end if;
                end if;
        end process;
    end generate;

    --Data input, block or distributed mode.
    in_bram_gen : if RAM_TYPE /= "flip-flop" generate
        input_bram_p : process(clka_i)
            begin
                if clka_i'event and clka_i = '1' then
                    if wrena_s = '1' and ena_s = '1' then
                        ram_sv(conv_integer(addra_s)) := dataa_i_s;
                    end if;
                end if;
        end process;
    end generate;

------------------------------------------------------------------------------------------------------------------------
--OUTPUT
------------------------------------------------------------------------------------------------------------------------
    --Asynchronous data output for flip-flop or distributed modes, ports A and B
    out_async_gen : if OUTPUT_SYNC = false and RAM_TYPE /= "block" generate
        dataa_o  <= ram_sv(conv_integer(addra_i));
        datab_o  <= ram_sv(conv_integer(addrb_i));
    end generate;

    --Synchronous data output for flip-flop or distributed modes
    out_sync_ffd_gen : if OUTPUT_SYNC = true and RAM_TYPE /= "block" generate
        --Synchronous data output for flip-flop or distributed modes, port A
        out_synca_ffd_p : process(clka_i, rsta_i)
            begin
                if rsta_i = '1' then
                    dataa_o <= (others=>'0');
                elsif clka_i'event and clka_i = '1' then
                    if ena_i = '1' then
                        dataa_o <= ram_sv(conv_integer(addra_i));
                    end if;
                end if;
        end process;

        --Synchronous data output for flip-flop or distributed modes, port B
        out_syncb_ffd_p : process(clkb_i, rstb_i)
            begin
                if rstb_i = '1' then
                    datab_o <= (others=>'0');
                elsif clkb_i'event and clkb_i = '1' then
                    if enb_i = '1' then
                        datab_o  <= ram_sv(conv_integer(addrb_i));
                    end if;
                end if;
        end process;

    end generate;

    --Synchronous data output for block mode
    out_bram_gen : if RAM_TYPE = "block" generate

        --Synchronous data output, modo block, port A
        out_synca_bram_p : process(clka_i)
            begin
                if clka_i'event and clka_i = '1' then
                    if ena_i = '1' then
                        if rsta_i = '1' then
                            dataa_o <= (others=>'0');
                        else
                            dataa_o <= ram_sv(conv_integer(addra_i));
                        end if;
                    end if;
                end if;
        end process;

        --Synchronous data output, modo block, port B
        out_syncb_bram_p : process(clkb_i)
            begin
                if clkb_i'event and clkb_i = '1' then
                    if enb_i = '1' then
                        if rstb_i = '1' then
                            datab_o  <= (others=>'0');
                        else
                            datab_o <= ram_sv(conv_integer(addrb_i));
                        end if;
                    end if;
                end if;
        end process;

    end generate;

end rtl;
