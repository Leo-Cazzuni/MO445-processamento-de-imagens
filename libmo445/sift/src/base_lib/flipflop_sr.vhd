------------------------------------------------------------------------------------------------------------------------
-- Single sentence description
------------------------------------------------------------------------------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
------------
-- Entity --
------------
entity flipflop_sr is
    generic (
        INITIAL_OUTPUT_STATE_C : std_logic := '0'
    );
    port (
        clk_i   : in  std_logic;
        arst_i  : in  std_logic;
        -- Control inputs
        set_i   : in  std_logic;
        reset_i : in  std_logic;
        -- Outputs
        q_o     : out std_logic;
        q_n_o   : out std_logic
    );
end flipflop_sr;

------------------
-- Architecture --
------------------
architecture rtl of flipflop_sr is
    signal q_s     : std_logic;

begin 
    q_o   <= q_s;
    q_n_o <= not q_s;

    process(clk_i, arst_i)
    begin
        if arst_i = '1' then
            q_s <= INITIAL_OUTPUT_STATE_C;
        elsif clk_i'event and clk_i = '1' then
            if reset_i = '1' then
                q_s <= '0';
            elsif set_i = '1' then
                q_s <= '1';
            end if;
        end if;
    end process;

end rtl;
