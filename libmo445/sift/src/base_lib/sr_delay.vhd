-----------------------------------------------------------------------------------------------------------------------
--Shift register based signal delay
-----------------------------------------------------------------------------------------------------------------------
--IEEE standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

------------
-- Entity --
------------
entity sr_delay is
    generic (
        DELAY_CYCLES_C : integer := 1;    -- Cycles to delay the output_o
        DATA_WIDTH_C   : integer := 1     -- Width of data ports
        );
    port (
        -- Clocking and resets
        clk_i         : in    std_logic;
        clk_en_i      : in    std_logic;
        -- Data ports
        input_i       : in    std_logic_vector(DATA_WIDTH_C - 1 downto 0);
        output_o      : out   std_logic_vector(DATA_WIDTH_C - 1 downto 0)
        );
end sr_delay;

------------------
-- Architecture --
------------------
architecture sr_delay of sr_delay is
    
    type data_type_t is array (natural range<>) of std_logic_vector(DATA_WIDTH_C - 1 downto 0);

    signal input_sr_s   : data_type_t(DELAY_CYCLES_C - 1 downto 0);

begin

    zero_delay : if DELAY_CYCLES_C = 0 generate
        output_o  <= input_i;
    end generate zero_delay;

    non_zero_delay : if DELAY_CYCLES_C /= 0 generate

        output_o  <= input_sr_s(DELAY_CYCLES_C - 1);

        process(clk_i)
        begin
            if clk_i'event and clk_i = '1' then
                if clk_en_i = '1' then
                    input_sr_s    <= input_sr_s(DELAY_CYCLES_C - 2 downto 0) & input_i;
                end if;
            end if;
        end process;
    end generate non_zero_delay;

end sr_delay;

