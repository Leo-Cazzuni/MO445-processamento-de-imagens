------------------------------------------------------------------------------------------------------------------------
--Synchronizer block and metastabilty remove
-- - Data rate must be at least 3 times less than sampling rate.
-- - Minimun pulse size must be at least 1,5 greater than a clock cycle
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity syncr is
    Port (
        I   : in  std_logic; --asynchronous input
        O   : out std_logic; --synchronous output
        CK  : in  std_logic  --output clock domain
    );
end syncr;

architecture rtl of syncr is

    signal tmp_s : std_logic_vector(1 downto 0);
    signal sync_input_tig_s : std_logic;

    attribute TIG : string;
    attribute TIG of sync_input_tig_s : signal is "TRUE";

begin

    sync_input_tig_s <= I;

    sync_ffd_p : process(CK)
        begin
            if CK'event and CK = '1' then
                tmp_s(1 downto 0) <= tmp_s(0) & sync_input_tig_s;
            end if;
    end process;

    O <= tmp_s(1);

end rtl;
