------------------------------------------------------------------------------------------------------------------------
-- Testbench for the synchronism library.
------------------------------------------------------------------------------------------------------------------------

-- IEEE standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
-- Sync library
library sync_lib;
use sync_lib.sync_lib_pkg.all;

entity sync_lib_tb is
end sync_lib_tb;

architecture behavioral of sync_lib_tb is

    signal mclk_i       : std_logic := '0';
-- 23 MHz signal to be synchronized.
    signal clk_s        : std_logic := '1';
-- 23 MHz signal already synchronized.
    signal sync_clk_s   : std_logic;
-- sync_clk_s's up detection.
    signal clk_up_s     : std_logic;
-- sync_clk_s's down detection.
    signal clk_dn_s     : std_logic;
-- sync_clk_s's up/down detection.
    signal clk_ud_s     : std_logic;
-- clk_s's sync and up detection.
    signal sync_up_s    : std_logic;
-- clk_s's sync and down detection.
    signal sync_dn_s    : std_logic;
-- clk_s's sync and up/down detection.
    signal sync_ud_s    : std_logic;
-- fast_clk_i width pulse
    signal pulse_i      : std_logic;
-- slow_clk_i width pulse
    signal pulse_o      : std_logic;

begin

    mclk_i <= not mclk_i after 5 ns; -- 100 MHz
    clk_s <= not clk_s after 21.739 ns; -- close to 23 MHz

    syncr_u: syncr
        port map(
            i   => clk_s,
            o   => sync_clk_s,
            ck  => mclk_i
        );

    det_up_u: det_up
        port map(
            i   => sync_clk_s,
            o   => clk_up_s,
            ck  => mclk_i
        );

    det_dn_u: det_dn
        port map(
            i   => sync_clk_s,
            o   => clk_dn_s,
            ck  => mclk_i
        );

    det_ud_u: det_ud
        port map(
            i   => sync_clk_s,
            o   => clk_ud_s,
            ck  => mclk_i
        );

    sync_det_up_u: sync_det_up
        port map(
            i   => clk_s,
            o   => sync_up_s,
            ck  => mclk_i
        );

    sync_det_dn_u: sync_det_dn
        port map(
            i   => clk_s,
            o   => sync_dn_s,
            ck  => mclk_i
        );

    sync_det_ud_u: sync_det_ud
        port map(
            i   => clk_s,
            o   => sync_ud_s,
            ck  => mclk_i
        );

    pulse_stretcher_u: pulse_stretcher
        port map(
            fast_clk_i  => mclk_i,
            slow_clk_i  => clk_s,
            pulse_i     => pulse_i,
            pulse_o     => pulse_o
        );

    pulse_i_p: process
        begin
            pulse_i <= '0';
            wait for 100 ns;
            while true loop
                wait until mclk_i'event and mclk_i = '1';
                pulse_i <= '1';
                wait until mclk_i'event and mclk_i = '1';
                pulse_i <= '0';
                wait for 200 ns;
            end loop;

        end process;


end behavioral;
