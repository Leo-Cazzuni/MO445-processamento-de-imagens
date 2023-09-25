------------------------------------------------------------------------------------------------------------------------


-- - Biblioteca: BASE_LIB
-- - Bloco: mem_TB_BE
-- - @autor Marcio Salvador
-- - @date 04 / 05 / 2015 - 11:18:25
-- Testbench for the base_lib memories.
------------------------------------------------------------------------------------------------------------------------

-- IEEE standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
-- Base library
library base_lib;
use base_lib.base_lib_pkg.all;
-- Test library
library check_lib;
use check_lib.check_lib_pkg.all;
-- PLB Lib
library plb_lib;
use plb_lib.plb_lib_pkg.all;
--DSP Library
--library dsp_lib;
--use dsp_lib.dsp_lib_pkg.all;

-----------------------------------------------------------------------------------------------------------------------
-- Test Bench
-----------------------------------------------------------------------------------------------------------------------
entity mem_TB_BE_PLB is
end entity mem_TB_BE_PLB;

architecture tb of mem_TB_BE_PLB is

    signal mclk_s     : std_logic := '1';
    signal arst_s     : std_logic := '1';
    signal plb_i_s    : plb_output_burst_t;
    signal plb_o_s    : plb_input_burst_t;
    --
    signal ena_s      : std_logic :='0';                                -- 1-bit A port enable input
    signal bea_s      : std_logic_vector(3 downto 0):=(others=>'0');    -- 4-bit A port write enable input
    signal dataa_i_s  : std_logic_vector(31 downto 0):=(others=>'0');   -- 32-bit A port data input
    signal dataa_o_s  : std_logic_vector(31 downto 0):=(others=>'0');   -- 32-bit A port data output
    signal addra_s    : std_logic_vector(9 downto 0):=(others=>'0');    -- 16-bit A port address input
    --
    signal enb_s      : std_logic := '0';                               -- 1-bit B port enable input
    signal beb_s      : std_logic_vector(3 downto 0):=(others=>'0');    -- 4-bit B port write enable input
    signal datab_i_s  : std_logic_vector(31 downto 0):=(others=>'0');   -- 32-bit B port data input
    signal datab_o_s  : std_logic_vector(31 downto 0):=(others=>'0');   -- 32-bit B port data output
    signal addrb_s    : std_logic_vector(9 downto 0):=(others=>'0');    -- 16-bit B port address input
    signal null_s     : std_logic_vector(0 downto 0):=(others=>'0');    -- 16-bit B port address input

begin

    mclk_s <= not mclk_s after 5 ns;
    arst_s <= '0' after 100 ns;

    plb_wr_gen_u: plb_wr_gen
    generic map(
        base_addr_c     => x"0000_0000",
        pkg_size_c      => 32,
        prbs_c          => true,
        test_sel_c      => 1
    )
    port map(
        mclk_i          => mclk_s,
        arst_i          => arst_s,
        prbs_rst_i      => arst_s, -- Reset exclusivo do PRBS
        block_en_i      => '1',
        plb_i           => plb_i_s,
        plb_o           => plb_o_s,
        eop_o           => open
    );

--------------------------------------------------------------------------------
-- plb_interface ---------------------------------------------------------------
    test_plb_u: plb_tdp_bram
    generic map(
        --Register Bank Constants
        base_addr_c     => x"0000_0000" --Device base address
    )
    port map(
        -- PLB Interface Signals
        plb_i          => plb_o_s,
        plb_o          => plb_i_s,
        -- B
        rst_i          => arst_s,
        clk_i          => mclk_s,
        wren_i         => beb_s,
        rden_i         => enb_s,
        data_i         => datab_o_s,
        data_o         => datab_i_s,
        addr_i         => addrb_s
    );

--------------------------------------------------------------------------------
-- dualport ram ----------------------------------------------------------------
    dualport_be_ram_u: true_dualportbe_ram
        port map(
            --Port A
            rsta_i    => arst_s,
            clka_i    => mclk_s,
            wrena_i   => bea_s,
            rdena_i   => ena_s,
            dataa_i   => dataa_i_s,
            dataa_o   => dataa_o_s,
            addra_i   => addra_s,
            --Port B
            rstb_i    => arst_s,
            clkb_i    => mclk_s,
            wrenb_i   => beb_s,
            rdenb_i   => enb_s,
            datab_i   => datab_i_s,
            datab_o   => datab_o_s,
            addrb_i   => addrb_s
        );

end tb;
