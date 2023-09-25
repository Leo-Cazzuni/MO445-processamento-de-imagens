-----------------------------------------------------------------------------------------
-- Este bloco realiza o cálculo do CRC de uma palavra de 32 bits de forma 
-- recursiva, ou seja, a cada nova palavra o CRC é calculado com base no
-- CRC anterior. O sinal crc_o representa o crc do frame a ser transmitido.
-----------------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity CRC32_gen is

    port
    (
        reset_i         : in  std_logic;
        clk_i           : in  std_logic;
        init_i          : in  std_logic; -- Pulso de início, deve ser pulsado antes do início da transmissão de cada pacote.
        data_i          : in  std_logic_vector(31 downto 0);
        data_valid_i    : in  std_logic; -- Indica se o dado é válido.
        CRC_o           : out std_logic_vector(31 downto 0) -- Representa o CRC de todos os dados recebidos desde o último init_i.
    );

end entity;

architecture rtl of CRC32_gen is

    signal CRC_s	: std_logic_vector(31 downto 0);
    -- Calcula o valor do CRC do dado recebido, baseado no crc anterior,
    -- quando aquele for o primeiro dado redebido o crc anterior deve ter todos os bits em 1.
    function	NextCRC_f(	
                        data_i  : in std_logic_vector(31 downto 0); 
                        crc_i   : in std_logic_vector(31 downto 0)
                )  return std_logic_vector is
	
        variable NewCRC_v   : std_logic_vector(31 downto 0);
        variable d_v        : std_logic_vector(31 downto 0);
        variable c_v        : std_logic_vector(31 downto 0);
                
    begin
    
        d_v   := data_i;
        c_v   := crc_i;
    
        newcrc_v( 0) := d_v(31) xor d_v(30) xor d_v(29) xor d_v(28) xor d_v(26) xor d_v(25) xor d_v(24) xor d_v(16) xor d_v(12) xor d_v(10) xor d_v( 9) xor d_v( 6) xor d_v( 0) xor c_v( 0) xor c_v( 6) xor c_v( 9) xor c_v(10) xor c_v(12) xor c_v(16) xor c_v(24) xor c_v(25) xor c_v(26) xor c_v(28) xor c_v(29) xor c_v(30) xor c_v(31);
        newcrc_v( 1) := d_v(28) xor d_v(27) xor d_v(24) xor d_v(17) xor d_v(16) xor d_v(13) xor d_v(12) xor d_v(11) xor d_v( 9) xor d_v( 7) xor d_v( 6) xor d_v( 1) xor d_v( 0) xor c_v( 0) xor c_v( 1) xor c_v( 6) xor c_v( 7) xor c_v( 9) xor c_v(11) xor c_v(12) xor c_v(13) xor c_v(16) xor c_v(17) xor c_v(24) xor c_v(27) xor c_v(28);
        newcrc_v( 2) := d_v(31) xor d_v(30) xor d_v(26) xor d_v(24) xor d_v(18) xor d_v(17) xor d_v(16) xor d_v(14) xor d_v(13) xor d_v( 9) xor d_v( 8) xor d_v( 7) xor d_v( 6) xor d_v( 2) xor d_v( 1) xor d_v( 0) xor c_v( 0) xor c_v( 1) xor c_v( 2) xor c_v( 6) xor c_v( 7) xor c_v( 8) xor c_v( 9) xor c_v(13) xor c_v(14) xor c_v(16) xor c_v(17) xor c_v(18) xor c_v(24) xor c_v(26) xor c_v(30) xor c_v(31);
        newcrc_v( 3) := d_v(31) xor d_v(27) xor d_v(25) xor d_v(19) xor d_v(18) xor d_v(17) xor d_v(15) xor d_v(14) xor d_v(10) xor d_v( 9) xor d_v( 8) xor d_v( 7) xor d_v( 3) xor d_v( 2) xor d_v( 1) xor c_v( 1) xor c_v( 2) xor c_v( 3) xor c_v( 7) xor c_v( 8) xor c_v( 9) xor c_v(10) xor c_v(14) xor c_v(15) xor c_v(17) xor c_v(18) xor c_v(19) xor c_v(25) xor c_v(27) xor c_v(31);
        newcrc_v( 4) := d_v(31) xor d_v(30) xor d_v(29) xor d_v(25) xor d_v(24) xor d_v(20) xor d_v(19) xor d_v(18) xor d_v(15) xor d_v(12) xor d_v(11) xor d_v( 8) xor d_v( 6) xor d_v( 4) xor d_v( 3) xor d_v( 2) xor d_v( 0) xor c_v( 0) xor c_v( 2) xor c_v( 3) xor c_v( 4) xor c_v( 6) xor c_v( 8) xor c_v(11) xor c_v(12) xor c_v(15) xor c_v(18) xor c_v(19) xor c_v(20) xor c_v(24) xor c_v(25) xor c_v(29) xor c_v(30) xor c_v(31);
        newcrc_v( 5) := d_v(29) xor d_v(28) xor d_v(24) xor d_v(21) xor d_v(20) xor d_v(19) xor d_v(13) xor d_v(10) xor d_v( 7) xor d_v( 6) xor d_v( 5) xor d_v( 4) xor d_v( 3) xor d_v( 1) xor d_v( 0) xor c_v( 0) xor c_v( 1) xor c_v( 3) xor c_v( 4) xor c_v( 5) xor c_v( 6) xor c_v( 7) xor c_v(10) xor c_v(13) xor c_v(19) xor c_v(20) xor c_v(21) xor c_v(24) xor c_v(28) xor c_v(29);
        newcrc_v( 6) := d_v(30) xor d_v(29) xor d_v(25) xor d_v(22) xor d_v(21) xor d_v(20) xor d_v(14) xor d_v(11) xor d_v( 8) xor d_v( 7) xor d_v( 6) xor d_v( 5) xor d_v( 4) xor d_v( 2) xor d_v( 1) xor c_v( 1) xor c_v( 2) xor c_v( 4) xor c_v( 5) xor c_v( 6) xor c_v( 7) xor c_v( 8) xor c_v(11) xor c_v(14) xor c_v(20) xor c_v(21) xor c_v(22) xor c_v(25) xor c_v(29) xor c_v(30);
        newcrc_v( 7) := d_v(29) xor d_v(28) xor d_v(25) xor d_v(24) xor d_v(23) xor d_v(22) xor d_v(21) xor d_v(16) xor d_v(15) xor d_v(10) xor d_v( 8) xor d_v( 7) xor d_v( 5) xor d_v( 3) xor d_v( 2) xor d_v( 0) xor c_v( 0) xor c_v( 2) xor c_v( 3) xor c_v( 5) xor c_v( 7) xor c_v( 8) xor c_v(10) xor c_v(15) xor c_v(16) xor c_v(21) xor c_v(22) xor c_v(23) xor c_v(24) xor c_v(25) xor c_v(28) xor c_v(29);
        newcrc_v( 8) := d_v(31) xor d_v(28) xor d_v(23) xor d_v(22) xor d_v(17) xor d_v(12) xor d_v(11) xor d_v(10) xor d_v( 8) xor d_v( 4) xor d_v( 3) xor d_v( 1) xor d_v( 0) xor c_v( 0) xor c_v( 1) xor c_v( 3) xor c_v( 4) xor c_v( 8) xor c_v(10) xor c_v(11) xor c_v(12) xor c_v(17) xor c_v(22) xor c_v(23) xor c_v(28) xor c_v(31);
        newcrc_v( 9) := d_v(29) xor d_v(24) xor d_v(23) xor d_v(18) xor d_v(13) xor d_v(12) xor d_v(11) xor d_v( 9) xor d_v( 5) xor d_v( 4) xor d_v( 2) xor d_v( 1) xor c_v( 1) xor c_v( 2) xor c_v( 4) xor c_v( 5) xor c_v( 9) xor c_v(11) xor c_v(12) xor c_v(13) xor c_v(18) xor c_v(23) xor c_v(24) xor c_v(29);
        newcrc_v(10) := d_v(31) xor d_v(29) xor d_v(28) xor d_v(26) xor d_v(19) xor d_v(16) xor d_v(14) xor d_v(13) xor d_v( 9) xor d_v( 5) xor d_v( 3) xor d_v( 2) xor d_v( 0) xor c_v( 0) xor c_v( 2) xor c_v( 3) xor c_v( 5) xor c_v( 9) xor c_v(13) xor c_v(14) xor c_v(16) xor c_v(19) xor c_v(26) xor c_v(28) xor c_v(29) xor c_v(31);
        newcrc_v(11) := d_v(31) xor d_v(28) xor d_v(27) xor d_v(26) xor d_v(25) xor d_v(24) xor d_v(20) xor d_v(17) xor d_v(16) xor d_v(15) xor d_v(14) xor d_v(12) xor d_v( 9) xor d_v( 4) xor d_v( 3) xor d_v( 1) xor d_v( 0) xor c_v( 0) xor c_v( 1) xor c_v( 3) xor c_v( 4) xor c_v( 9) xor c_v(12) xor c_v(14) xor c_v(15) xor c_v(16) xor c_v(17) xor c_v(20) xor c_v(24) xor c_v(25) xor c_v(26) xor c_v(27) xor c_v(28) xor c_v(31);
        newcrc_v(12) := d_v(31) xor d_v(30) xor d_v(27) xor d_v(24) xor d_v(21) xor d_v(18) xor d_v(17) xor d_v(15) xor d_v(13) xor d_v(12) xor d_v( 9) xor d_v( 6) xor d_v( 5) xor d_v( 4) xor d_v( 2) xor d_v( 1) xor d_v( 0) xor c_v( 0) xor c_v( 1) xor c_v( 2) xor c_v( 4) xor c_v( 5) xor c_v( 6) xor c_v( 9) xor c_v(12) xor c_v(13) xor c_v(15) xor c_v(17) xor c_v(18) xor c_v(21) xor c_v(24) xor c_v(27) xor c_v(30) xor c_v(31);
        newcrc_v(13) := d_v(31) xor d_v(28) xor d_v(25) xor d_v(22) xor d_v(19) xor d_v(18) xor d_v(16) xor d_v(14) xor d_v(13) xor d_v(10) xor d_v( 7) xor d_v( 6) xor d_v( 5) xor d_v( 3) xor d_v( 2) xor d_v( 1) xor c_v( 1) xor c_v( 2) xor c_v( 3) xor c_v( 5) xor c_v( 6) xor c_v( 7) xor c_v(10) xor c_v(13) xor c_v(14) xor c_v(16) xor c_v(18) xor c_v(19) xor c_v(22) xor c_v(25) xor c_v(28) xor c_v(31);
        newcrc_v(14) := d_v(29) xor d_v(26) xor d_v(23) xor d_v(20) xor d_v(19) xor d_v(17) xor d_v(15) xor d_v(14) xor d_v(11) xor d_v( 8) xor d_v( 7) xor d_v( 6) xor d_v( 4) xor d_v( 3) xor d_v( 2) xor c_v( 2) xor c_v( 3) xor c_v( 4) xor c_v( 6) xor c_v( 7) xor c_v( 8) xor c_v(11) xor c_v(14) xor c_v(15) xor c_v(17) xor c_v(19) xor c_v(20) xor c_v(23) xor c_v(26) xor c_v(29);
        newcrc_v(15) := d_v(30) xor d_v(27) xor d_v(24) xor d_v(21) xor d_v(20) xor d_v(18) xor d_v(16) xor d_v(15) xor d_v(12) xor d_v( 9) xor d_v( 8) xor d_v( 7) xor d_v( 5) xor d_v( 4) xor d_v( 3) xor c_v( 3) xor c_v( 4) xor c_v( 5) xor c_v( 7) xor c_v( 8) xor c_v( 9) xor c_v(12) xor c_v(15) xor c_v(16) xor c_v(18) xor c_v(20) xor c_v(21) xor c_v(24) xor c_v(27) xor c_v(30);
        newcrc_v(16) := d_v(30) xor d_v(29) xor d_v(26) xor d_v(24) xor d_v(22) xor d_v(21) xor d_v(19) xor d_v(17) xor d_v(13) xor d_v(12) xor d_v( 8) xor d_v( 5) xor d_v( 4) xor d_v( 0) xor c_v( 0) xor c_v( 4) xor c_v( 5) xor c_v( 8) xor c_v(12) xor c_v(13) xor c_v(17) xor c_v(19) xor c_v(21) xor c_v(22) xor c_v(24) xor c_v(26) xor c_v(29) xor c_v(30);
        newcrc_v(17) := d_v(31) xor d_v(30) xor d_v(27) xor d_v(25) xor d_v(23) xor d_v(22) xor d_v(20) xor d_v(18) xor d_v(14) xor d_v(13) xor d_v( 9) xor d_v( 6) xor d_v( 5) xor d_v( 1) xor c_v( 1) xor c_v( 5) xor c_v( 6) xor c_v( 9) xor c_v(13) xor c_v(14) xor c_v(18) xor c_v(20) xor c_v(22) xor c_v(23) xor c_v(25) xor c_v(27) xor c_v(30) xor c_v(31);
        newcrc_v(18) := d_v(31) xor d_v(28) xor d_v(26) xor d_v(24) xor d_v(23) xor d_v(21) xor d_v(19) xor d_v(15) xor d_v(14) xor d_v(10) xor d_v( 7) xor d_v( 6) xor d_v( 2) xor c_v( 2) xor c_v( 6) xor c_v( 7) xor c_v(10) xor c_v(14) xor c_v(15) xor c_v(19) xor c_v(21) xor c_v(23) xor c_v(24) xor c_v(26) xor c_v(28) xor c_v(31);
        newcrc_v(19) := d_v(29) xor d_v(27) xor d_v(25) xor d_v(24) xor d_v(22) xor d_v(20) xor d_v(16) xor d_v(15) xor d_v(11) xor d_v( 8) xor d_v( 7) xor d_v( 3) xor c_v( 3) xor c_v( 7) xor c_v( 8) xor c_v(11) xor c_v(15) xor c_v(16) xor c_v(20) xor c_v(22) xor c_v(24) xor c_v(25) xor c_v(27) xor c_v(29);
        newcrc_v(20) := d_v(30) xor d_v(28) xor d_v(26) xor d_v(25) xor d_v(23) xor d_v(21) xor d_v(17) xor d_v(16) xor d_v(12) xor d_v( 9) xor d_v( 8) xor d_v( 4) xor c_v( 4) xor c_v( 8) xor c_v( 9) xor c_v(12) xor c_v(16) xor c_v(17) xor c_v(21) xor c_v(23) xor c_v(25) xor c_v(26) xor c_v(28) xor c_v(30);
        newcrc_v(21) := d_v(31) xor d_v(29) xor d_v(27) xor d_v(26) xor d_v(24) xor d_v(22) xor d_v(18) xor d_v(17) xor d_v(13) xor d_v(10) xor d_v( 9) xor d_v( 5) xor c_v( 5) xor c_v( 9) xor c_v(10) xor c_v(13) xor c_v(17) xor c_v(18) xor c_v(22) xor c_v(24) xor c_v(26) xor c_v(27) xor c_v(29) xor c_v(31);
        newcrc_v(22) := d_v(31) xor d_v(29) xor d_v(27) xor d_v(26) xor d_v(24) xor d_v(23) xor d_v(19) xor d_v(18) xor d_v(16) xor d_v(14) xor d_v(12) xor d_v(11) xor d_v( 9) xor d_v( 0) xor c_v( 0) xor c_v( 9) xor c_v(11) xor c_v(12) xor c_v(14) xor c_v(16) xor c_v(18) xor c_v(19) xor c_v(23) xor c_v(24) xor c_v(26) xor c_v(27) xor c_v(29) xor c_v(31);
        newcrc_v(23) := d_v(31) xor d_v(29) xor d_v(27) xor d_v(26) xor d_v(20) xor d_v(19) xor d_v(17) xor d_v(16) xor d_v(15) xor d_v(13) xor d_v( 9) xor d_v( 6) xor d_v( 1) xor d_v( 0) xor c_v( 0) xor c_v( 1) xor c_v( 6) xor c_v( 9) xor c_v(13) xor c_v(15) xor c_v(16) xor c_v(17) xor c_v(19) xor c_v(20) xor c_v(26) xor c_v(27) xor c_v(29) xor c_v(31);
        newcrc_v(24) := d_v(30) xor d_v(28) xor d_v(27) xor d_v(21) xor d_v(20) xor d_v(18) xor d_v(17) xor d_v(16) xor d_v(14) xor d_v(10) xor d_v( 7) xor d_v( 2) xor d_v( 1) xor c_v( 1) xor c_v( 2) xor c_v( 7) xor c_v(10) xor c_v(14) xor c_v(16) xor c_v(17) xor c_v(18) xor c_v(20) xor c_v(21) xor c_v(27) xor c_v(28) xor c_v(30);
        newcrc_v(25) := d_v(31) xor d_v(29) xor d_v(28) xor d_v(22) xor d_v(21) xor d_v(19) xor d_v(18) xor d_v(17) xor d_v(15) xor d_v(11) xor d_v( 8) xor d_v( 3) xor d_v( 2) xor c_v( 2) xor c_v( 3) xor c_v( 8) xor c_v(11) xor c_v(15) xor c_v(17) xor c_v(18) xor c_v(19) xor c_v(21) xor c_v(22) xor c_v(28) xor c_v(29) xor c_v(31);
        newcrc_v(26) := d_v(31) xor d_v(28) xor d_v(26) xor d_v(25) xor d_v(24) xor d_v(23) xor d_v(22) xor d_v(20) xor d_v(19) xor d_v(18) xor d_v(10) xor d_v( 6) xor d_v( 4) xor d_v( 3) xor d_v( 0) xor c_v( 0) xor c_v( 3) xor c_v( 4) xor c_v( 6) xor c_v(10) xor c_v(18) xor c_v(19) xor c_v(20) xor c_v(22) xor c_v(23) xor c_v(24) xor c_v(25) xor c_v(26) xor c_v(28) xor c_v(31);
        newcrc_v(27) := d_v(29) xor d_v(27) xor d_v(26) xor d_v(25) xor d_v(24) xor d_v(23) xor d_v(21) xor d_v(20) xor d_v(19) xor d_v(11) xor d_v( 7) xor d_v( 5) xor d_v( 4) xor d_v( 1) xor c_v( 1) xor c_v( 4) xor c_v( 5) xor c_v( 7) xor c_v(11) xor c_v(19) xor c_v(20) xor c_v(21) xor c_v(23) xor c_v(24) xor c_v(25) xor c_v(26) xor c_v(27) xor c_v(29);
        newcrc_v(28) := d_v(30) xor d_v(28) xor d_v(27) xor d_v(26) xor d_v(25) xor d_v(24) xor d_v(22) xor d_v(21) xor d_v(20) xor d_v(12) xor d_v( 8) xor d_v( 6) xor d_v( 5) xor d_v( 2) xor c_v( 2) xor c_v( 5) xor c_v( 6) xor c_v( 8) xor c_v(12) xor c_v(20) xor c_v(21) xor c_v(22) xor c_v(24) xor c_v(25) xor c_v(26) xor c_v(27) xor c_v(28) xor c_v(30);
        newcrc_v(29) := d_v(31) xor d_v(29) xor d_v(28) xor d_v(27) xor d_v(26) xor d_v(25) xor d_v(23) xor d_v(22) xor d_v(21) xor d_v(13) xor d_v( 9) xor d_v( 7) xor d_v( 6) xor d_v( 3) xor c_v( 3) xor c_v( 6) xor c_v( 7) xor c_v( 9) xor c_v(13) xor c_v(21) xor c_v(22) xor c_v(23) xor c_v(25) xor c_v(26) xor c_v(27) xor c_v(28) xor c_v(29) xor c_v(31);
        newcrc_v(30) := d_v(30) xor d_v(29) xor d_v(28) xor d_v(27) xor d_v(26) xor d_v(24) xor d_v(23) xor d_v(22) xor d_v(14) xor d_v(10) xor d_v( 8) xor d_v( 7) xor d_v( 4) xor c_v( 4) xor c_v( 7) xor c_v( 8) xor c_v(10) xor c_v(14) xor c_v(22) xor c_v(23) xor c_v(24) xor c_v(26) xor c_v(27) xor c_v(28) xor c_v(29) xor c_v(30);
        newcrc_v(31) := d_v(31) xor d_v(30) xor d_v(29) xor d_v(28) xor d_v(27) xor d_v(25) xor d_v(24) xor d_v(23) xor d_v(15) xor d_v(11) xor d_v( 9) xor d_v( 8) xor d_v( 5) xor c_v( 5) xor c_v( 8) xor c_v( 9) xor c_v(11) xor c_v(15) xor c_v(23) xor c_v(24) xor c_v(25) xor c_v(27) xor c_v(28) xor c_v(29) xor c_v(30) xor c_v(31);

        return(NewCRC_v);
    end NextCRC_f;
	
begin

    CRC_o <= CRC_s;

    crc_calc_p : process(clk_i, reset_i)
    begin
        if (reset_i = '1') then
            CRC_s <= (others => '1');
        elsif clk_i'event and clk_i = '1' then
            if (init_i = '1') then
                CRC_s <= (others => '1');
            elsif (data_valid_i = '1') then
                CRC_s <= NextCRC_f(data_i, CRC_s);
            end if;
        end if;
    end process crc_calc_p;
			
end rtl;

