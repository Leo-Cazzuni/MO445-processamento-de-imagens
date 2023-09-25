-----------------------------------------------------------------------------------------------------------------------
-- Generates a power-on reset. May be cascaded by using the trigger_i
-----------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.std_logic_arith.all;
    use ieee.std_logic_unsigned.all;
--Basic library
library base_lib;
    use base_lib.base_lib_pkg.all;
library sync_lib;

-- Declaração da entidade.
entity porst_generator is
    generic (
        ACTIVE_CYCLES_C : integer   := 16;          -- Minimum number of active rst cycles
        TRIGGER_TYPE_C  : string    := "ACTIVE_HIGH"-- RISING_EDGE, FALLING_EDGE, ACTIVE_LOW, ACTIVE_HIGH
        );
    port (
        -- Clock inputs
        clk_i       : in    std_logic;
        clk_en_i    : in    std_logic;

        trigger_i   : in    std_logic;  -- The number of ACTIVE_CYCLES_C will start to count whenever this signals goes active according to TRIGGER_TYPE_C
        porst_o     : out   std_logic   -- Power-on reset generated output
        );

end porst_generator;


-- Corpo da arquitetura do bloco, aqui é descrito o funcionamento interno do bloco
architecture rtl of porst_generator is

    signal porst_cnt_s    : std_logic_vector(vec_fit(ACTIVE_CYCLES_C) - 1 downto 0) := (others => '0');
    signal trigger_r_s    : std_logic;
    signal trigger_f_s    : std_logic;
    signal trigger_sel_s  : std_logic;

    signal porst_int_s    : std_logic := '1';

begin

    trigger_sel_s <=    trigger_r_s      when TRIGGER_TYPE_C = "RISING_EDGE"     else
                        trigger_f_s      when TRIGGER_TYPE_C = "FALLING_EDGE"    else
                        trigger_i      when TRIGGER_TYPE_C = "ACTIVE_HIGH"     else
                        not trigger_i  when TRIGGER_TYPE_C = "ACTIVE_LOW"      else
                        '1';

    porst_o <= porst_int_s;

    hold_rst_edges_gen : entity sync_lib.edge_detector
        generic map (
            SYNC_INPUT_C        => true,
            OUTPUT_REG_NUM_C    => 2
            )
        port map (
            -- Clocking and resets
            clk_i       => clk_i,
            clk_en_i    => clk_en_i,

            --
            input_i     => trigger_i, 
            rising_o    => trigger_r_s, 
            falling_o   => trigger_f_s, 
            toggle_o    => open 
        );

    process(clk_i)
    begin
        if clk_i'event and clk_i = '1' then
            if clk_en_i = '1' then
                if porst_cnt_s = 0 then
                    if trigger_sel_s = '1' then
                        porst_cnt_s   <= porst_cnt_s + 1;
                    end if;
                elsif porst_cnt_s /= ACTIVE_CYCLES_C - 1 then
                    porst_cnt_s   <= porst_cnt_s + 1;
                elsif porst_cnt_s = ACTIVE_CYCLES_C - 1 then
                    porst_int_s   <= '0';
                end if;
            end if;
        end if;
    end process;

end rtl;
