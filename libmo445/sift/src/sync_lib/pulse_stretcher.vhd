------------------------------------------------------------------------------------------------------------------------
-- Transforms a single fast_clk_i period pulse in a single slow_clk_i period pulse.
-- If the pulse comes from a faster clock domain the syncr block can't be used.
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
--Synchronism Library
library sync_lib;
use sync_lib.sync_lib_pkg.all;

entity pulse_stretcher is
    port(
        fast_clk_i  : in  std_logic;--Fast clock
        slow_clk_i  : in  std_logic;--Slow clock
        pulse_i     : in  std_logic;--Input pulse (fast_clk domain)
        pulse_o     : out std_logic --Output pulse (slow_clk domain)
    );
end pulse_stretcher;

architecture rtl of pulse_stretcher is

    signal stretch_s        : std_logic := '0';
    signal stretch_slow_s   : std_logic := '0';
    signal ack_s            : std_logic := '0';

begin

-- This signal stays high until the end of the domain transference.
    stretch_p: process(fast_clk_i)
        begin
            if fast_clk_i'event and fast_clk_i = '1' then
                if ack_s = '1' then
                    stretch_s <= '0';
                elsif pulse_i = '1' then
                    stretch_s <= '1';
                end if;
            end if;
        end process;

    stretch_slow_u: syncr
        port map(
            ck  => slow_clk_i,
            i   => stretch_s,
            o   => stretch_slow_s
        );

    ack_u: syncr
        port map(
            ck  => fast_clk_i,
            i   => stretch_slow_s,
            o   => ack_s
        );

    pulse_o_u: det_up
        port map(
            ck  => slow_clk_i,
            i   => stretch_slow_s,
            o   => pulse_o
        );

end rtl;
