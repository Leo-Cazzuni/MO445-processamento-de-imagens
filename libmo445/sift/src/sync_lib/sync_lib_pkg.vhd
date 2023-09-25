------------------------------------------------------------------------------------------------------------------------
-- Synchronization and pulse detection
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

package sync_lib_pkg is

    --General synchronizer
    component syncr
        Port (
            I   : in  std_logic;
            O   : out std_logic;
            CK  : in  std_logic
        );
    end component;

    --Up edge detector
    component det_up
        Port (
            I   : in  std_logic;
            O   : out std_logic;
            CK  : in  std_logic
        );
    end component;

    --Down edge detector
    component det_dn
        Port (
            I   : in  std_logic;
            O   : out std_logic;
            CK  : in  std_logic
        );
    end component;

    --Up and down edge detector
    component det_ud
        Port (
            I   : in  std_logic;
            O   : out std_logic;
            CK  : in  std_logic
        );
    end component;

    --Asynchronous edge up detector
    component sync_det_up
        Port (
            I   : in  std_logic;
            O   : out std_logic;
            CK  : in  std_logic
        );
    end component;

    --Asynchronous edge down detector
    component sync_det_dn
        Port (
            I   : in  std_logic;
            O   : out std_logic;
            CK  : in  std_logic
        );
    end component;

    --Asynchronous edge up and down detector
    component sync_det_ud
        Port (
            I   : in  std_logic;
            O   : out std_logic;
            CK  : in  std_logic
        );
    end component;

    --Transforms a single fast_clk_i period pulse in a single slow_clk_i period pulse.
    component pulse_stretcher
        port(
            fast_clk_i  : in  std_logic;--Fast clock
            slow_clk_i  : in  std_logic;--Slow clock
            pulse_i     : in  std_logic;--Input pulse (fast_clk domain)
            pulse_o     : out std_logic --Output pulse (slow_clk domain)
        );
    end component;

end sync_lib_pkg;

package body sync_lib_pkg is
end sync_lib_pkg;
