------------------------------------------------------------------------------------------------------------------------
-- Asynchronous edge down detector
-- Pule for output edge deteced will happen about 3 to 4 clock cycles after the actual edge.
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

entity sync_det_dn is
    Port (
        I   : in  std_logic;--Async input
        O   : out std_logic;--Pulse output
        CK  : in  std_logic --Output clock domain
    );
end sync_det_dn;

architecture rtl of sync_det_dn is

    signal tmp_s : std_logic_vector(2 downto 0);
    signal sync_input_tig_s : std_logic;

    attribute TIG : string;
    attribute TIG of sync_input_tig_s : signal is "TRUE";

begin

    sync_input_tig_s <= I;

    sync_ffd_p : process(CK)
        begin
            if CK'event and CK = '1' then
                tmp_s(2 downto 0) <= tmp_s(1 downto 0) & sync_input_tig_s;
            end if;
    end process;

    O <= tmp_s(2) and (not tmp_s(1));

end rtl;
