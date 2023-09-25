------------------------------------------------------------------------------------------------------------------------
--Decimation block.
------------------------------------------------------------------------------------------------------------------------
-- IEEE standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

--Decimation block.

--This block reduces the original sampling rate for 2 sequence sources to a lower rate.
--This 2 sequence sources are downsampled and multiplexed in DATA_A_O port.
--DATA_B_O is used only when the decimation has factor 1. It means that the full data rate from A and B input port are output in DATA_A_O and DATA_B_O, respectively.
--When even decimaton factor is used, ONLY DATA_A_I input port is downsampled and output.

entity decimation is

  generic (
        DataSize             :  integer := 14;  --Bit vector size of input data port.
        DecimationSize       :  integer := 16   --Bit vector size of decimation.
          );

     port (
        CLK_I                :  in std_logic;   --Clock.
        RST_I                :  in std_logic;   --Active-high reset.

        DATA_A_I             :  in std_logic_vector(DataSize-1 downto 0);        --Input Data A.
        DATA_B_I             :  in std_logic_vector(DataSize-1 downto 0);        --Input Data B.
        OVR_A_I              :  in std_logic;                                    --overange input a.
        OVR_B_I              :  in std_logic;                                    --overange input b.
        GATE_I               :  in std_logic;                                    --Enable the data aquisition in the block.

        DECIMATION_I         :  in std_logic_vector(DecimationSize-1 downto 0);  --Decimation reduces the original sampling rate for a sequence to a lower rate.

        DATA_A_O             :  out std_logic_vector(DataSize-1 downto 0);       --Output Data A.
        DATA_B_O             :  out std_logic_vector(DataSize-1 downto 0);       --Output Data B port ONLY used when decimation has factor 1.
        OVR_A_O              :  out std_logic;                                   --overange output a.
        OVR_B_O              :  out std_logic;                                   --overange output b.
        DATA_VALID_A_O       :  out std_logic;                                   --Data Valid A at output.
        DATA_VALID_B_O       :  out std_logic                                    --Data Valid B at output.

          );
end;

architecture behaviour of decimation is

signal data_a_s               : std_logic_vector(DataSize-1 downto 0);           -- Data A registred.
signal data_b_s               : std_logic_vector(DataSize-1 downto 0);           -- Data B registred.
signal ovr_s                  : std_logic;
signal cnt_decim_s            : std_logic_vector(DecimationSize-1 downto 0);     -- Decimation counter.
signal cnt_decim_div2_s       : std_logic_vector(DecimationSize-1 downto 0);     -- Half decimation counter.
signal decimation_s           : std_logic_vector(DecimationSize-1 downto 0);     -- Decimation-1 registred.
signal decimation_div2_s      : std_logic_vector(DecimationSize-1 downto 0);     -- Decimation/2-1 registred.
signal gate_s                 : std_logic;                                       -- Gate registred.
signal data_valid_a_s         : std_logic;                                       -- Data Valid A.
signal data_valid_b_s         : std_logic;                                       -- Data Valid B.

begin

    --Input registration
    input_p: process (CLK_I,RST_I)
    begin
        if (RST_I ='1') then
            data_a_s      <=  (others => '0');
            data_b_s      <=  (others => '0');
            gate_s        <=  '0';
        elsif (CLK_I='1' and CLK_I'Event) then
            data_a_s      <=  DATA_A_I;
            data_b_s      <=  DATA_B_I;
            gate_s        <=  GATE_I;
        end if;
    end process;

    --Half Decimation Counter
    --This process count continously up to decimation/2 -1.
    halfdeci_p: process (CLK_I,RST_I)
    begin
        if (RST_I ='1') then
            cnt_decim_div2_s       <=  (others => '1');
            decimation_div2_s      <=  (others => '0');
        elsif (CLK_I='1' and CLK_I'Event) then
            decimation_div2_s(DecimationSize-1 downto 0)   <=  '0' & decimation_s(DecimationSize-1 downto 1);   -- decimation_s/2
                if GATE_I = '1' then
                    if cnt_decim_div2_s = decimation_div2_s then
                        cnt_decim_div2_s          <=  (others => '0');
                    else
                        cnt_decim_div2_s          <=  cnt_decim_div2_s + 1;
                    end if;
                else
                    cnt_decim_div2_s          <=  (others => '1');
                end if;
        end if;
    end process;

    --Decimation Counter
    --This process count continously up to decimation-1.
    deci_p: process (CLK_I,RST_I)
    begin
        if (RST_I ='1') then
            cnt_decim_s       <=  (others => '1');
            decimation_s      <=  (others => '0');
        elsif (CLK_I='1' and CLK_I'Event) then
            decimation_s        <=  DECIMATION_I - 1;   -- decimation-1
                if GATE_I = '1' then
                    if cnt_decim_s = decimation_s then
                        cnt_decim_s          <=  (others => '0');
                    else
                        cnt_decim_s          <=  cnt_decim_s + 1;
                    end if;
                else
                    cnt_decim_s          <=  (others => '1');
                end if;
        end if;
    end process;

    --Rate controller of DATA_A_I port
    data_valid_a_s <= '1' and gate_s when DECIMATION_I(0) = '1' and cnt_decim_s = 0       else    -- when decimation is odd factor.
                      '1' and gate_s when DECIMATION_I(0) = '0' and cnt_decim_div2_s = 0  else    -- when decimation is even factor.
                      '0';

    --Rate controller of DATA_B_I port
    data_valid_b_s <= '1' and gate_s when DECIMATION_I(0) = '1' and cnt_decim_s = decimation_div2_s  else    -- when decimation is odd factor.
                      '0';                                                                                   -- when decimation is even factor.

    --This block must not miss an overange information. If an overange happens in a sample that is not going to the output, the next sample must indicate
    --the overange.
    ovr_p: process(rst_i, clk_i)
        begin
            if rst_i = '1' then
                ovr_s <= '0';
            elsif clk_i'event and clk_i = '1' then
                if gate_i = '1' and (ovr_a_i = '1' or ovr_b_i = '1') then
                    ovr_s <= '1';
                elsif data_valid_a_s = '1' or data_valid_b_s = '1' then
                    ovr_s <= '0';
                elsif gate_i = '0' then
                    ovr_s <= '0';
                end if;
            end if;
        end process;

    --Multiplexer
    --The mux output in DATA_A_O the down sampled data from A and B input port.
    --DATA_B_O is used only when the decimation has factor 1. I means that the full data rate from A and B input port are output in DATA_A_O and DATA_B_O, respectively.
    mux_p: process (CLK_I,RST_I)
    begin
        if (RST_I ='1') then
            DATA_A_O            <= (others => '0');
            DATA_B_O            <= (others => '0');
            OVR_A_O             <= '0';
            OVR_B_O             <= '0';
            DATA_VALID_A_O      <= '0';
            DATA_VALID_B_O      <= '0';
        elsif (CLK_I='1' and CLK_I'Event) then
            if gate_s = '1' then
                if decimation_s = 0 then
                    DATA_A_O            <= data_a_s;
                    DATA_B_O            <= data_b_s;
                    OVR_A_O             <= ovr_s;
                    OVR_B_O             <= ovr_s;
                    DATA_VALID_A_O      <= data_valid_a_s;
                    DATA_VALID_B_O      <= data_valid_b_s;
                elsif data_valid_a_s = '1' then
                    DATA_A_O            <= data_a_s;
                    OVR_A_O             <= ovr_s;
                    DATA_VALID_A_O      <= data_valid_a_s;
                else
                    DATA_A_O            <= data_b_s;
                    OVR_A_O             <= ovr_s;
                    DATA_VALID_A_O      <= data_valid_b_s;
                end if;
            else
                DATA_A_O            <= (others => '0');
                DATA_B_O            <= (others => '0');
                OVR_A_O             <= '0';
                OVR_B_O             <= '0';
                DATA_VALID_A_O      <= '0';
                DATA_VALID_B_O      <= '0';
            end if;
        end if;
    end process;

end behaviour;
