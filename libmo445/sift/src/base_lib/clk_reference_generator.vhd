------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
------------------------
--Standard Libraries --
------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
-- base lib
library base_lib;
use base_lib.base_lib_pkg.all;

------------
-- Entity --
------------
entity clk_reference_generator is
    generic (
        DIVISION_VALUE_C : integer := 4;
        OUTPUT_MODE_C    : string  := "PULSE"   -- PULSE or CLOCK
    );
    port (
        arst_i      : in  std_logic;
        clk_i       : in  std_logic;
        clk_en_i    : in  std_logic;
        -- Generated reference
        ref_o       : out std_logic
    );

end clk_reference_generator;


architecture behaviour of clk_reference_generator is

    signal ref_cnt_s : std_logic_vector(vec_fit(DIVISION_VALUE_C) - 1 downto 0);
    signal ref_s     : std_logic;

begin 

    ref_o   <= ref_s;

    ---------------
    -- Processes --
    ---------------
    process(arst_i, clk_i)
    begin
        if arst_i = '1' then
            ref_cnt_s   <= (others => '0');
            ref_s       <= '0';
        elsif clk_i'event and clk_i = '1' then
            if clk_en_i = '1' then
                if OUTPUT_MODE_C = "CLOCK" then
                    if ref_cnt_s = DIVISION_VALUE_C/2 - 1 then
                        ref_cnt_s   <= (others => '0');
                        ref_s       <= not ref_s;
                    else
                        ref_cnt_s   <= ref_cnt_s + 1;
                    end if;
                elsif OUTPUT_MODE_C = "PULSE" then
                    ref_s       <= '0';
                    if ref_cnt_s = DIVISION_VALUE_C - 1 then
                        ref_cnt_s   <= (others => '0');
                        ref_s       <= '1';
                    else
                        ref_cnt_s   <= ref_cnt_s + 1;
                    end if;
                end if;
            end if;

        end if;
    end process;

end behaviour;


