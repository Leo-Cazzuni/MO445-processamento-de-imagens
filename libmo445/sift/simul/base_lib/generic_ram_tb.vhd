------------------------------------------------------------------------------------------------------------------------
--  Single sentence description
------------------------------------------------------------------------------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.std_logic_arith.all;
    use ieee.std_logic_unsigned.all;

library base_lib;
    use base_lib.base_lib_pkg.all;

library check_lib;

------------
-- Entity --
------------
entity generic_ram_tb is
    generic (
        REF_WDATA_A_TEST_SEL   : integer := 7;
        REF_ADDR_A_TEST_SEL    : integer := 6;
        REF_ENABLES_A_TEST_SEL : integer := 5;
        REF_ENABLE_B_TEST_SEL  : integer := 4;
        REF_ADDR_B_TEST_SEL    : integer := 6
    );
end generic_ram_tb;


------------------
-- Architecture --
------------------
architecture rtl of generic_ram_tb is

    -----------
    -- Types --
    -----------

    ---------------
    -- Constants --
    ---------------
    constant OUTPUT_SYNC : boolean := True;

    constant ADDR_SIZE_C : integer := 8;
    constant PORT_SIZE_C : integer := 8;

    constant REF_CLK_A_PERIOD_C : time := 12 ns;
    constant REF_CLK_B_PERIOD_C : time := 10 ns;

    constant ERROR_CNT_LIMIT : integer := 5;

    -------------
    -- Signals --
    -------------
    signal ref_rst_a_s               : std_logic := '1';
    signal ref_rst_a0_s              : std_logic := '1';
    signal ref_rst_b_s               : std_logic := '1';
    signal ref_rst_b0_s              : std_logic := '1';
    signal ref_clk_a_s               : std_logic := '1';
    signal ref_clk_b_s               : std_logic := '1';

    signal ref_wren_a_s              : std_logic;
    signal ref_en_a_s                : std_logic;
    signal ref_en_b_s                : std_logic;
    signal ref_addr_a_s              : std_logic_vector(ADDR_SIZE_C - 1 downto 0);
    signal ref_wdata_a_s             : std_logic_vector(PORT_SIZE_C - 1 downto 0);

    signal ref_ff_rdata_a_s          : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal sv_ff_rdata_a_s           : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal ff_rdata_a_diff          : std_logic;
    signal sv_distributed_rdata_a_s  : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal ref_distributed_rdata_a_s : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal distributed_rdata_a_diff : std_logic;
    signal sv_block_rdata_a_s        : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal ref_block_rdata_a_s       : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal block_rdata_a_diff       : std_logic;

    signal ref_addr_b_s              : std_logic_vector(ADDR_SIZE_C - 1 downto 0);

    signal ref_ff_rdata_b_s          : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal sv_ff_rdata_b_s           : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal ff_rdata_b_diff          : std_logic;
    signal ref_distributed_rdata_b_s : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal sv_distributed_rdata_b_s  : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal distributed_rdata_b_diff : std_logic;
    signal ref_block_rdata_b_s       : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal sv_block_rdata_b_s        : std_logic_vector(PORT_SIZE_C - 1 downto 0);
    signal block_rdata_b_diff       : std_logic;

begin

    -------------------
    -- Port mappings --
    -------------------
    ref_wdata_a_gen_u : entity check_lib.parallel_prbs_gen
        generic map (
            TEST_SEL_C      => REF_WDATA_A_TEST_SEL,
            PORT_SIZE_C     => PORT_SIZE_C
        )
        port map (
            rst_i           => ref_rst_a0_s,
            mclk_i          => ref_clk_a_s,
            --
            error_insert_i  => '0',
            fix_patt_i      => (others => '0'),
            --
            oen_i           => '1',
            data_o          => ref_wdata_a_s
        );

    ref_addr_a_gen : entity check_lib.parallel_prbs_gen
        generic map (
            TEST_SEL_C      => REF_ADDR_A_TEST_SEL,
            PORT_SIZE_C     => PORT_SIZE_C
        )
        port map (
            rst_i           => ref_rst_a0_s,
            mclk_i          => ref_clk_a_s,
            --
            error_insert_i  => '0',
            fix_patt_i      => (others => '0'),
            --
            oen_i           => '1',
            data_o          => ref_addr_a_s
        );

    ref_wr_wren_a_gen : entity check_lib.parallel_prbs_gen
        generic map (
            TEST_SEL_C      => REF_ENABLES_A_TEST_SEL,
            PORT_SIZE_C     => 2
        )
        port map (
            rst_i           => ref_rst_a0_s,
            mclk_i          => ref_clk_a_s,
            --
            error_insert_i  => '0',
            fix_patt_i      => (others => '0'),
            --
            oen_i           => '1',
            data_o(0)       => ref_wren_a_s,
            data_o(1)       => ref_en_a_s
        );

    ref_en_b_gen : entity check_lib.parallel_prbs_gen
        generic map (
            TEST_SEL_C      => REF_ENABLE_B_TEST_SEL,
            PORT_SIZE_C     => 1
        )
        port map (
            rst_i           => ref_rst_b0_s,
            mclk_i          => ref_clk_b_s,
            --
            error_insert_i  => '0',
            fix_patt_i      => (others => '0'),
            --
            oen_i           => '1',
            data_o(0)       => ref_en_b_s
        );

    ref_addr_b_gen : entity check_lib.parallel_prbs_gen
        generic map (
            TEST_SEL_C      => REF_ADDR_B_TEST_SEL,
            PORT_SIZE_C     => ADDR_SIZE_C
        )
        port map (
            rst_i           => ref_rst_b0_s,
            mclk_i          => ref_clk_b_s,
            --
            error_insert_i  => '0',
            fix_patt_i      => (others => '0'),
            --
            oen_i           => '1',
            data_o          => ref_addr_b_s
        );

    -------------------------
    -- Current generic RAM --
    -------------------------
    ref_ff_u : entity base_lib.generic_ram
        generic map (
            ADDR_SIZE_C => ADDR_SIZE_C,
            PORT_SIZE_C => PORT_SIZE_C,
            OUTPUT_SYNC => True,
            RAM_TYPE    => "flip-flop"
        )
        port map (
            rsta_i   => ref_rst_a_s,
            clka_i   => ref_clk_a_s,
            wrena_i  => ref_wren_a_s,
            ena_i    => ref_en_a_s,
            dataa_i  => ref_wdata_a_s,
            dataa_o  => ref_ff_rdata_a_s,
            addra_i  => ref_addr_a_s,

            rstb_i   => ref_rst_b_s,
            clkb_i   => ref_clk_b_s,
            enb_i    => ref_en_b_s,
            datab_o  => ref_ff_rdata_b_s,
            addrb_i  => ref_addr_b_s
        );

    ref_distributed_u : entity base_lib.generic_ram
        generic map (
            ADDR_SIZE_C => ADDR_SIZE_C,
            PORT_SIZE_C => PORT_SIZE_C,
            OUTPUT_SYNC => True,
            RAM_TYPE    => "distributed"
        )
        port map (
            rsta_i   => ref_rst_a_s,
            clka_i   => ref_clk_a_s,
            wrena_i  => ref_wren_a_s,
            ena_i    => ref_en_a_s,
            dataa_i  => ref_wdata_a_s,
            dataa_o  => ref_distributed_rdata_a_s,
            addra_i  => ref_addr_a_s,

            rstb_i   => ref_rst_b_s,
            clkb_i   => ref_clk_b_s,
            enb_i    => ref_en_b_s,
            datab_o  => ref_distributed_rdata_b_s,
            addrb_i  => ref_addr_b_s
        );

    ref_block_u : entity base_lib.generic_ram
        generic map (
            ADDR_SIZE_C => ADDR_SIZE_C,
            PORT_SIZE_C => PORT_SIZE_C,
            OUTPUT_SYNC => False,
            RAM_TYPE    => "block"
        )
        port map (
            rsta_i   => ref_rst_a_s,
            clka_i   => ref_clk_a_s,
            wrena_i  => ref_wren_a_s,
            ena_i    => ref_en_a_s,
            dataa_i  => ref_wdata_a_s,
            dataa_o  => ref_block_rdata_a_s,
            addra_i  => ref_addr_a_s,

            rstb_i   => ref_rst_b_s,
            clkb_i   => ref_clk_b_s,
            enb_i    => ref_en_b_s,
            datab_o  => ref_block_rdata_b_s,
            addrb_i  => ref_addr_b_s
        );

    ref_block_samp_u : entity base_lib.generic_ram
        generic map (
            ADDR_SIZE_C         => ADDR_SIZE_C,
            PORT_SIZE_C         => PORT_SIZE_C,
            OUTPUT_SYNC         => False,
            RAM_TYPE            => "block",
            DOUBLE_SAMP_EN_G    => true
        )
        port map (
            rsta_i   => ref_rst_a_s,
            clka_i   => ref_clk_a_s,
            wrena_i  => ref_wren_a_s,
            ena_i    => ref_en_a_s,
            dataa_i  => ref_wdata_a_s,
            dataa_o  => open,
            addra_i  => ref_addr_a_s,

            rstb_i   => ref_rst_b_s,
            clkb_i   => ref_clk_b_s,
            enb_i    => ref_en_b_s,
            datab_o  => open,
            addrb_i  => ref_addr_b_s
        );
    --------------------------
    -- Proposed generic RAM --
    --------------------------
    sv_ff_u : entity base_lib.generic_ram_sv
        generic map (
            ADDR_SIZE_C => ADDR_SIZE_C,
            PORT_SIZE_C => PORT_SIZE_C,
            OUTPUT_SYNC => True,
            RAM_TYPE    => "flip-flop"
        )
        port map (
            rsta_i   => ref_rst_a_s,
            clka_i   => ref_clk_a_s,
            wrena_i  => ref_wren_a_s,
            ena_i    => ref_en_a_s,
            dataa_i  => ref_wdata_a_s,
            dataa_o  => sv_ff_rdata_a_s,
            addra_i  => ref_addr_a_s,

            rstb_i   => ref_rst_b_s,
            clkb_i   => ref_clk_b_s,
            enb_i    => ref_en_b_s,
            datab_o  => sv_ff_rdata_b_s,
            addrb_i  => ref_addr_b_s
        );

    sv_distributed_u : entity base_lib.generic_ram_sv
        generic map (
            ADDR_SIZE_C => ADDR_SIZE_C,
            PORT_SIZE_C => PORT_SIZE_C,
            OUTPUT_SYNC => True,
            RAM_TYPE    => "distributed"
        )
        port map (
            rsta_i   => ref_rst_a_s,
            clka_i   => ref_clk_a_s,
            wrena_i  => ref_wren_a_s,
            ena_i    => ref_en_a_s,
            dataa_i  => ref_wdata_a_s,
            dataa_o  => sv_distributed_rdata_a_s,
            addra_i  => ref_addr_a_s,

            rstb_i   => ref_rst_b_s,
            clkb_i   => ref_clk_b_s,
            enb_i    => ref_en_b_s,
            datab_o  => sv_distributed_rdata_b_s,
            addrb_i  => ref_addr_b_s
        );

    sv_block_u : entity base_lib.generic_ram_sv
        generic map (
            ADDR_SIZE_C => ADDR_SIZE_C,
            PORT_SIZE_C => PORT_SIZE_C,
            OUTPUT_SYNC => False,
            RAM_TYPE    => "block"
        )
        port map (
            rsta_i   => ref_rst_a_s,
            clka_i   => ref_clk_a_s,
            wrena_i  => ref_wren_a_s,
            ena_i    => ref_en_a_s,
            dataa_i  => ref_wdata_a_s,
            dataa_o  => sv_block_rdata_a_s,
            addra_i  => ref_addr_a_s,

            rstb_i   => ref_rst_b_s,
            clkb_i   => ref_clk_b_s,
            enb_i    => ref_en_b_s,
            datab_o  => sv_block_rdata_b_s,
            addrb_i  => ref_addr_b_s
        );

    ------------------------------
    -- Asynchronous assignments --
    ------------------------------
    ref_clk_a_s  <= not ref_clk_a_s after REF_CLK_A_PERIOD_C/2;
    ref_rst_a_s  <= '1', '0' after 10*REF_CLK_A_PERIOD_C;
    ref_rst_a0_s <= '1', '0' after 20*REF_CLK_A_PERIOD_C;

    ref_clk_b_s  <= not ref_clk_b_s after REF_CLK_B_PERIOD_C/2;
    ref_rst_b_s  <= '1', '0' after 10*REF_CLK_B_PERIOD_C;
    ref_rst_b0_s <= '1', '0' after 20*REF_CLK_B_PERIOD_C;

    ---------------
    -- Processes --
    ---------------
    process(ref_clk_a_s, ref_rst_a0_s)
        variable error_cnt : integer;
    begin
        if ref_rst_a0_s = '1' then
            ff_rdata_a_diff          <= '0';
            distributed_rdata_a_diff <= '0';
            block_rdata_a_diff       <= '0';
            error_cnt    := 0;
        elsif ref_clk_a_s'event and ref_clk_a_s = '1' then
            ff_rdata_a_diff          <= '0';
            distributed_rdata_a_diff <= '0';
            block_rdata_a_diff       <= '0';
            if ref_en_a_s = '1' then

                if ref_ff_rdata_a_s /= sv_ff_rdata_a_s then
                    ff_rdata_a_diff <= '1';
                end if;

                if ref_distributed_rdata_a_s /= sv_distributed_rdata_a_s then
                    distributed_rdata_a_diff <= '1';
                end if;

                if ref_block_rdata_a_s /= sv_block_rdata_a_s then
                    block_rdata_a_diff <= '1';
                end if;

            end if;

            if (ff_rdata_a_diff          /= '0' or
                distributed_rdata_a_diff /= '0' or
                block_rdata_a_diff       /= '0') then

                report "Errors at port A" severity Note;
                error_cnt := error_cnt + 1;
            end if;

            if ref_rst_a_s = '0' then
                assert error_cnt < ERROR_CNT_LIMIT
                    report "Port A error"
                    severity Failure;
            end if;
        end if;
    end process;

    process(ref_clk_b_s, ref_rst_a0_s)
        variable error_cnt : integer;
    begin
        if ref_rst_a0_s = '1' then
            ff_rdata_b_diff          <= '0';
            distributed_rdata_b_diff <= '0';
            block_rdata_b_diff       <= '0';
            error_cnt    := 0;
        elsif ref_clk_b_s'event and ref_clk_b_s = '1' then
            ff_rdata_b_diff          <= '0';
            distributed_rdata_b_diff <= '0';
            block_rdata_b_diff       <= '0';
            if ref_en_b_s = '1' then

                if ref_ff_rdata_b_s /= sv_ff_rdata_b_s then
                    ff_rdata_b_diff <= '1';
                end if;

                if ref_distributed_rdata_b_s /= sv_distributed_rdata_b_s then
                    distributed_rdata_b_diff <= '1';
                end if;

                if ref_block_rdata_b_s /= sv_block_rdata_b_s then
                    block_rdata_b_diff <= '1';
                end if;

            end if;

            if (ff_rdata_b_diff          /= '0' or
                distributed_rdata_b_diff /= '0' or
                block_rdata_b_diff       /= '0') then

                error_cnt := error_cnt + 1;
                report "Errors at port B" severity Note;
            end if;

            if ref_rst_b_s = '0' then
                assert error_cnt < ERROR_CNT_LIMIT
                    report "Port B error"
                    severity Failure;
            end if;
        end if;
    end process;

end rtl;
