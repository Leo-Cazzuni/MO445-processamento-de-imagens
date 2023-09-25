------------------------------------------------------------------------------------------------------------------------
-- crc_gen and crc_check - testbench.
------------------------------------------------------------------------------------------------------------------------
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
--
library base_lib;
use base_lib.base_lib_pkg.all;

entity crc_tb is
end crc_tb;


architecture rtl of crc_tb is
    
    signal mclk_s               : std_logic := '0';
    signal rst_s                : std_logic;
    signal Init_s               : std_logic;
    
    signal data_s               : std_logic_vector(31 downto 0);
    signal data_valid_s         : std_logic;    
    signal crc_gen_s            : std_logic_vector(31 downto 0);
    
        
    signal data_valid_check_s   : std_logic;
    signal data_check_s         : std_logic_vector(31 downto 0);
    signal data_last_s          : std_logic;
    
    signal crc_err_s            : std_logic;
    signal crc_err_valid_s      : std_logic;
    
begin

    rst_p: process
        begin
            rst_s <= '1';
            wait for 30 ns;
            rst_s <= '0';
            wait;
        end process;

    mclk_s <= not mclk_s after 5 ns; -- 100 MHz

    process
    begin
        data_valid_s        <= '0';
        Init_s              <= '0';
        data_last_s         <= '0';
        wait for 40 ns;
        
        wait until mclk_s'event and mclk_s = '1';
        Init_s              <= '1'; -- pulso de início do frame
        
        wait until mclk_s'event and mclk_s = '1';
        Init_s              <= '0';
        data_s              <= x"01234567"; -- dado 1 para crc gen            
        data_valid_s        <= '1';
        
        wait until mclk_s'event and mclk_s = '1';
        data_valid_check_s  <= '1';
        data_check_s        <= x"01234567"; -- dado 1 para crc check
        --data_check_s        <= x"574A2C8B";
        data_s              <= x"89ABCDEF"; -- dado 2 para crc gen
        data_valid_s        <= '0';
        
        wait until mclk_s'event and mclk_s = '1';
        data_valid_s        <= '1';
        data_check_s        <= x"89ABCDEF"; -- dado 2 para crc check
        data_valid_check_s  <= '0';
        
        wait until mclk_s'event and mclk_s = '1';
        data_valid_check_s  <= '1';
        
        wait until mclk_s'event and mclk_s = '1';
        data_check_s        <= crc_gen_s; -- crc gerado pelo crc gen e enviado para crc check
        data_last_s         <= '1';
        
        
        wait until mclk_s'event and mclk_s = '1';
        data_last_s         <= '0';
        
        wait until crc_err_valid_s = '1';
        assert crc_err_s = '0' report "### CRC ERROR ###"  severity note;
        assert crc_err_s = '1' report "### CRC OK ###"     severity note;
        wait;
    end process;
    

    CRC32_gen_u : CRC32_gen
        port map(
            reset_i         => rst_s,
            clk_i           => mclk_s,
            init_i          => Init_s,
            data_i          => data_s,
            data_valid_i    => data_valid_s,
            
            CRC_o           => crc_gen_s
        );
        
    CRC32_check_u : CRC32_check
        port map(
        reset_i         => rst_s,
        clk_i           => mclk_s,
        data_i          => data_check_s,
        init_i          => Init_s,
        data_valid_i    => data_valid_check_s,

        data_last_i     => data_last_s,
        
        crc_err_valid_o => crc_err_valid_s,
        crc_err_o       => crc_err_s
        );        

end rtl;