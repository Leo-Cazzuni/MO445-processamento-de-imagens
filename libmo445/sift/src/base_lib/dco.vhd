------------------------------------------------------------------------------------------------------------------------
-- Digitally controlled oscillator
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
------------------------------------------------------------------------------------------------------------------------
-- Digitally controlled oscillator

-- The DCO is a K module accumulator. It works by increasing its value by N, and thus dividing the master clock or a
-- prescaler, if present.
--
-- USE:
-- - Master clock division by fractionary number
--  - \f$ Fout = \frac{Fmclk*N}{2^K} \f$
-- - Prescale frequency division by fractionary number
--  - \f$ Fout = \frac{Fscale*N}{2^K} \f$
-- - Where:
--  - \f$ Fmclk \f$: Nominal master clock frequency
--  - \f$ Fscale \f$: Nominal prescale clock frequency
--  - \f$ K \f$: DCO module size
--  - \f$ N \f$: Proportional frequency constant.
--  - \f$ Fout \f$:  Desired output frequency
-- - Fout Precision ():
--  - To \f$ Fmclk \f$: \f$ \delta = \frac{Fmclk}{2^K} \f$
--  - To \f$ Fscale \f$: \f$ \delta = \frac{Fscale}{2^K} \f$
--
-- NOTES:
-- - increase the value of k generates the most accurate results, however, the counter can be huge and not be feasible.
-- - if the prescaler input is not used, it must be stucked to '1'.
-- - calculation:
--  - First define the precision (in general, it is given in ppm). from it, we obtain k.
--  - With the value of k, use the formula to determine N. Fout and Fmclk / Fscale obviously must already be known.
------------------------------------------------------------------------------------------------------------------------
entity dco is
    Generic (
        K_c         : integer := 8   --DCO internal counter size (module)
    );
    Port (
        clk_i       : in  std_logic;                        --Master clock
        resync_i    : in  std_logic;                        --DCO resynchronization output
        prescale_i  : in  std_logic;                        --Prescale frequency division
        N_i         : in  std_logic_vector(K_c-1 downto 0); --Proportional frequency constant
        clk_o       : out std_logic                         --Clock output
    );
end dco;

architecture rtl of dco is

    signal dco_cnt : std_logic_vector(k_c-1 downto 0) := (others=>'0');

begin

    --N mode counter, with resynch and enable port
    dco_p : process(clk_i)
        begin
            if clk_i'event and clk_i = '1' then
                if resync_i = '1' then
                    dco_cnt <= (others=>'0');
                elsif prescale_i = '1' then
                    dco_cnt <= dco_cnt + N_i;
                end if;
            end if;
    end process;

    clk_o <= dco_cnt(dco_cnt'high);


end rtl;
