-----------------------------------------------------------------------------------------------------------------------
-- Detect edges of a given signal. Optionally synchronizes the input_i and/or delays the output_o
-----------------------------------------------------------------------------------------------------------------------
--IEEE standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library sync_lib;
use sync_lib.syncr;

library base_lib;

------------
-- Entity --
------------
entity edge_detector is
    generic (
        SYNC_INPUT_C        : boolean   := true;
        OUTPUT_REG_NUM_C    : integer   := 0
    );
    port (
        -- Clocking and resets
        clk_i       : in    std_logic;
        clk_en_i    : in    std_logic;

        -- Data input_i
        input_i     : in    std_logic;
        -- Data output_o
        rising_o    : out   std_logic;
        falling_o   : out   std_logic;
        toggle_o    : out   std_logic
    );

end edge_detector;

------------------
-- Architecture --
------------------
architecture edge_detector of edge_detector is

    signal input_s      : std_logic;        -- The actual input, sychronized or not
    signal input_d0_s   : std_logic := '0'; -- input_s delayed by 1 cycle

    -- Asynchronous edges detected
    signal rising_s     : std_logic;
    signal falling_s    : std_logic;
    signal toggle_s     : std_logic;

begin

    -- Selects the correct input_i based on SYNC_INPUT_C generic
    gsi : if SYNC_INPUT_C generate
        i_sync : entity sync_lib.syncr
        port map (
            CK  => clk_i,
            I   => input_i,
            O   => input_s
        );
    end generate gsi;

    ngsi : if not SYNC_INPUT_C generate
        input_s   <= input_i;
    end generate ngsi;

    -- Effectivelly detect the signal edges.
    -- Notice we don't compare to '1' due to issues for transitions originating
    -- from signals in 'X' (undefined), 'Z' (high impedance), 'H' (tri-state high)
    rising_s    <= '1'  when input_s /= '0' and input_d0_s = '0' else '0';
    falling_s   <= '1'  when input_s = '0' and input_d0_s /= '0' else '0';
    toggle_s    <= rising_s or falling_s;

    -- Creates the delay chain for rising_o, falling_o and edges
    delay_gen : entity base_lib.sr_delay
        generic map (
            DELAY_CYCLES_C  => OUTPUT_REG_NUM_C,
            DATA_WIDTH_C    => 3
            )
        port map (
            clk_i       => clk_i,
            clk_en_i    => clk_en_i,

            input_i(0)  => rising_s,
            input_i(1)  => falling_s,
            input_i(2)  => toggle_s,
            output_o(0) => rising_o,
            output_o(1) => falling_o,
            output_o(2) => toggle_o
        );


    process(clk_i)
    begin
        if clk_i'event and clk_i = '1' then
            if clk_en_i = '1' then
                input_d0_s    <= input_s;
            end if;
        end if;
    end process;

end edge_detector;

