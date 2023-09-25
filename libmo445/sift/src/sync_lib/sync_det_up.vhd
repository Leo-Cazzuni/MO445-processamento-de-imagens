------------------------------------------------------------------------------------------------------------------------
-- Asynchronous edge up detector
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- - Pule for output edge deteced will happen about 3 to 4 clock cycles after the actual edge.
entity sync_det_up is
    Port (
        ck  : in  std_logic;--Output clock domain
        i   : in  std_logic;--Async input
        o   : out std_logic --Pulse output
    );
end sync_det_up;

architecture rtl of sync_det_up is

    signal tmp_s : std_logic_vector(2 downto 0);
    signal sync_input_tig_s : std_logic;

    attribute TIG : string;
    attribute TIG of sync_input_tig_s : signal is "TRUE";

begin

    sync_input_tig_s <= i;

    sync_ffd_p : process(ck)
        begin
            if ck'event and ck = '1' then
                tmp_s(2 downto 0) <= tmp_s(1 downto 0) & sync_input_tig_s;
            end if;
    end process;

    o <= (not tmp_s(2)) and tmp_s(1);

end rtl;
