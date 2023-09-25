library std;
use std.standard.all;

library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

library base_lib;
use base_lib.base_lib_pkg.all;

library axi_stream_lib;
use axi_stream_lib.axi_stream_lib_pkg.all;

library check_lib;
use check_lib.check_lib_pkg.all;

library sync_lib;
use sync_lib.sync_lib_pkg.all;

entity tb_decimation is

end tb_decimation;

architecture behaviour of tb_decimation is

component deci_test

     port (
        CLK_I                :  in std_logic;   --Clock.
        RST_I                :  in std_logic;   --Active-high reset.

        DATA_I               :  in std_logic_vector(13 downto 0);        --Input Data A.
        DATA_VALID_I         :  in std_logic;                                    --Enable the data aquisition in the block.

        DECIMATION_I         :  in std_logic_vector(15 downto 0);  --Decimation reduces the original sampling rate for a sequence to a lower rate.

        DATA_TEST_O          :  out std_logic_vector(13 downto 0)       --Output Data B port ONLY used when decimation has factor 1.

          );

end component;

constant pkg_c           : integer := 1024;

signal clk               : std_logic := '0';
signal rst               : std_logic := '1';
signal gate_s            : std_logic := '0';
signal cnt_gate_s        : integer;

signal decimation_s      : std_logic_vector(15 downto 0) := x"0001";
signal datah_s           : std_logic_vector(13 downto 0);
signal datal_s           : std_logic_vector(13 downto 0);
signal data_deci_s       : std_logic_vector(13 downto 0);
signal dv_deci_s         : std_logic := '0';
signal data_s            : std_logic_vector(31 downto 0);
signal enable_s          : std_logic;
signal last_s            : std_logic;
signal timer_s           : std_logic;
signal cnt               : unsigned(15 downto 0);
signal null_s            : std_logic_vector(7 downto 0);
signal deci_pulse_s      : std_logic;

begin

clock : process
   begin
       wait for 10 ns; clk  <= not clk;
   end process clock;

reset : process
   begin
   rst  <= '1'; wait for 20 ns;
   rst  <= '0';
   wait;
end process reset;


inst_decimation: decimation

    port map (
        CLK_I               =>  clk,
        RST_I               =>  rst,

        DATA_A_I            =>  data_s(13 downto 0),
        DATA_B_I            =>  (others => '0'),
        GATE_I              =>  timer_s,

        DECIMATION_I        =>  decimation_s,

        DATA_A_O            =>  data_deci_s,
        DATA_B_O            =>  open,
        DATA_VALID_A_O      =>  dv_deci_s,
        DATA_VALID_B_O      =>  open

          );

axi_stream_gen_u: axi_stream_gen
generic map(
    tdata_test_sel_c        => 1,
    tvalid_test_sel_c       => 0,
    pkt_size_c              => pkg_c,
    tuser_size_c            => 32,
    tdata_size_c            => 8
)
port map(
    arst_i                  => rst,
    aclk_i                  => clk,
    -- AXI
    tdata_o                 => null_s,
    tuser_o                 => data_s,
    tvalid_o                => enable_s,
    tlast_o                 => last_s,
    tready_i                => timer_s
);

cnt_p: process(clk,rst)
begin
    if rst = '1' then
        cnt <= (others => '0');
    elsif clk'event and clk = '1' then
        if cnt = pkg_c - 1 then
            cnt <= (others => '0');
        else
            cnt <= cnt + 1;
        end if;
    end if;
end process;

timer_p: process(clk, rst)
begin
    if rst = '1' then
        timer_s <= '0';
    elsif clk'event and clk = '1' then
        if cnt = pkg_c - 1 then
            timer_s <= not timer_s;
        end if;
    end if;
end process;

det_dn_u: det_dn
port map(
    I   => timer_s,
    O   => deci_pulse_s,
    CK  => clk
);

decimation_p: process(clk, rst)
begin
    if rst = '1' then
        decimation_s <= x"0001";
    elsif clk'event and clk = '1' then
        if deci_pulse_s = '1' then
            decimation_s <= decimation_s(14 downto 0) & decimation_s(15);
        end if;
    end if;
end process;

end behaviour;
