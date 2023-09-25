------------------------------------------------------------------------------------------------------------------------
-- Generic RAM.
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
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
--
-- Notes:
-- - Blockram will always be of synchronous output.
-- - Memory behavior may change due to parameter selection.
-- - Memory clock and total possible size is (and will always be) family dependent.
------------------------------------------------------------------------------------------------------------------------
entity generic_ram is
    Generic (
        ADDR_SIZE_C         : integer := 8;      --Address bus size
        PORT_SIZE_C         : integer := 8;      --Data bus size
        OUTPUT_SYNC         : boolean := true;   --Output mode selection
        RAM_TYPE            : string  := "block";--Memory type selection
        DOUBLE_SAMP_EN_G    : boolean := false   --Habilita amostragem do dado de saida
    );
    Port (
        rsta_i   : in  std_logic;                                   -- Port A reset
        clka_i   : in  std_logic;                                   -- Port A clock
        wrena_i  : in  std_logic;                                   -- Port A write enable
        ena_i    : in  std_logic;                                   -- Port A enable
        dataa_i  : in  std_logic_vector(PORT_SIZE_C-1 downto 0);    -- Port A input data bus
        dataa_o  : out std_logic_vector(PORT_SIZE_C-1 downto 0);    -- Port A output data bus
        addra_i  : in  std_logic_vector(ADDR_SIZE_C-1 downto 0);    -- Port A address bus
        rstb_i   : in  std_logic;                                   -- Port B reset
        clkb_i   : in  std_logic;                                   -- Port B clock
        enb_i    : in  std_logic;                                   -- Port B enable
        datab_o  : out std_logic_vector(PORT_SIZE_C-1 downto 0);    -- Port B output data bus
        addrb_i  : in  std_logic_vector(ADDR_SIZE_C-1 downto 0)     -- Port B address bus
    );
end generic_ram;

architecture rtl of generic_ram is

    constant RAM_SIZE_C       : integer := (2**ADDR_SIZE_C) * PORT_SIZE_C;

    -- RAMB36 has 36kb (1024 positions of 36 bits). Varies if PlanAhead uses true dual port, simple dual port, etc
    constant BLOCK_RAM_SIZE_C              : integer := 36*1024;
    constant DISTRIBUTED_UPPER_TRESHOLD_C  : integer := 10*BLOCK_RAM_SIZE_C;
    constant RAMB_LOWER_THRESHOLD_C        : integer := BLOCK_RAM_SIZE_C/4;

    type   ram_t is array (2**ADDR_SIZE_C-1 downto 0) of std_logic_vector(PORT_SIZE_C-1 downto 0);
    signal ram_s : ram_t := (others => (others => '0'));
    --shared variable ram_s : ram_t := (others => (others => '0'));

    signal dataa_s  : std_logic_vector(PORT_SIZE_C-1 downto 0) := (others => '0');
    signal datab_s  : std_logic_vector(PORT_SIZE_C-1 downto 0) := (others => '0');

    attribute RAM_STYLE : string;
    attribute RAM_STYLE of ram_s : signal is RAM_TYPE;

begin

    assert RAM_TYPE = "flip-flop" or RAM_TYPE = "block" or RAM_TYPE = "distributed" or
           RAM_TYPE = "auto"
        report "Value '" & RAM_TYPE & "' for generic RAM_TYPE is invalid. " &
               "Should be 'block', 'distributed', 'flip-flop' or 'auto'"
        severity Failure;

    -- Report if the RAM size is too big for distributed or small for block
    -- On PlanAhead report, look for XST warning #1749 ("WARNING:Xst:1749") in
    -- the synthesis report

    assert not (RAM_TYPE /= "block" and RAM_SIZE_C > DISTRIBUTED_UPPER_TRESHOLD_C)
        report "RAM size of " & integer'image(RAM_SIZE_C) & " is too large for RAM type " & RAM_TYPE & ". "
        severity Warning;

    assert not (RAM_TYPE = "block" and RAM_SIZE_C < RAMB_LOWER_THRESHOLD_C)
        report "RAM size of " & integer'image(RAM_SIZE_C) & " is too small for RAM type " & RAM_TYPE & ". "
        severity Warning;

------------------------------------------------------------------------------------------------------------------------
--INPUT
------------------------------------------------------------------------------------------------------------------------
    --Data input, flip-flop mode.
    --There is full memory reset.
    in_ffd_gen : if RAM_TYPE = "flip-flop" generate
        input_ffd_p : process(clka_i, rsta_i)
            begin
                if rsta_i = '1' then
                    ram_s <= (others=>(others=>'0'));
                elsif clka_i'event and clka_i = '1' then
                    if wrena_i = '1' and ena_i = '1' then
                        ram_s(conv_integer(addra_i)) <= dataa_i;
                    end if;
                end if;
        end process;
    end generate;

    --Data input, block or distributed mode.
    in_bram_gen : if RAM_TYPE /= "flip-flop" generate
        input_bram_p : process(clka_i)
            begin
                if clka_i'event and clka_i = '1' then
                    if wrena_i = '1' and ena_i = '1' then
                        ram_s(conv_integer(addra_i)) <= dataa_i;
                    end if;
                end if;
        end process;
    end generate;

------------------------------------------------------------------------------------------------------------------------
--OUTPUT
------------------------------------------------------------------------------------------------------------------------
    --Asynchronous data output for flip-flop or distributed modes, ports A and B
    out_async_gen : if not OUTPUT_SYNC and RAM_TYPE /= "block" and RAM_TYPE /= "auto" generate
        dataa_o <= ram_s(conv_integer(addra_i));
        datab_o <= ram_s(conv_integer(addrb_i));
    end generate;

    --Synchronous data output for flip-flop or distributed modes
    out_sync_ffd_gen : if OUTPUT_SYNC and RAM_TYPE /= "block" and RAM_TYPE /= "auto" generate
        --Synchronous data output for flip-flop or distributed modes, port A
        out_synca_ffd_p : process(clka_i, rsta_i)
            begin
                if rsta_i = '1' then
                    dataa_o <= (others=>'0');
                elsif clka_i'event and clka_i = '1' then
                    if ena_i = '1' then
                        dataa_o <= ram_s(conv_integer(addra_i));
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
                        datab_o <= ram_s(conv_integer(addrb_i));
                    end if;
                end if;
        end process;

    end generate;

    --Synchronous data output for block mode
    out_bram_gen : if RAM_TYPE = "block" or RAM_TYPE = "auto" generate

        single_samp_gen: if DOUBLE_SAMP_EN_G = false generate

            --Synchronous data output, modo block, port A
            out_synca_bram_p : process(clka_i)
                begin
                    if clka_i'event and clka_i = '1' then
                        if ena_i = '1' then
                            if rsta_i = '1' then
                                dataa_o <= (others=>'0');
                            else
                                dataa_o <= ram_s(conv_integer(addra_i));
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
                                datab_o <= (others=>'0');
                            else
                                datab_o <= ram_s(conv_integer(addrb_i));
                            end if;
                        end if;
                    end if;
                end process;

        end generate single_samp_gen;

        double_samp_gen: if DOUBLE_SAMP_EN_G = true generate

            --Synchronous data output, modo block, port A
            out_synca_bram_p : process(clka_i)
                begin
                    if clka_i'event and clka_i = '1' then
                        if ena_i = '1' then
                            if rsta_i = '1' then
                                --dataa_o <= (others=>'0');
                                dataa_s <= (others => '0');
                            else
                                dataa_s <= ram_s(conv_integer(addra_i));
                            end if;
                        end if;
                    dataa_o <= dataa_s;
                    end if;
                end process;

            --Synchronous data output, modo block, port B
            out_syncb_bram_p : process(clkb_i)
                begin
                    if clkb_i'event and clkb_i = '1' then
                        if enb_i = '1' then
                            if rstb_i = '1' then
                                --datab_o <= (others=>'0');
                                datab_s <= (others => '0');
                            else
                                datab_s <= ram_s(conv_integer(addrb_i));
                            end if;
                        end if;
                    datab_o <= datab_s;
                    end if;
                end process;

        end generate double_samp_gen;

    end generate;

end rtl;
