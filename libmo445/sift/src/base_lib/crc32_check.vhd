--------------------------------------------------------------------------------------------
-- Este bloco realiza o cálculo do CRC de uma palavra de 32 bits de forma 
-- recursiva, ou seja, a cada nova palavra o CRC é calculado com base no
-- CRC anterior. O cálculo de todo o frame recebido, incluindo o campo de 
-- CRC, deve ser 0. Caso contrário o sinal crc_err_o é setado.
-------------------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- base lib
library base_lib;
use base_lib.base_lib_pkg.all;

entity CRC32_check is
    port
    (
        reset_i         : in std_logic;
        clk_i           : in std_logic;
        init_i          : in std_logic; -- Pulso de início, deve ser pulsado antes do início da recepção de cada pacote.
        --        
        data_i          : in std_logic_vector(31 downto 0);
        data_valid_i    : in std_logic; -- Indica se o dado é válido.
        data_last_i     : in std_logic; -- Indica se o dado recebido é o último dado do pacote, ou seja, se o dado corresponde ao CRC do frame a ser checado.
        --
        crc_err_valid_o : out std_logic;-- Indica se o sinal crc_err_o é válido, ele é setado no segundo ciclo após a recepção do último dado do pacote.
        crc_err_o       : out std_logic -- Quando 1, ele representa um erro de checagem do CRC.
    );

end entity;

architecture rtl of CRC32_check is

    signal CRC_s		: std_logic_vector(31 downto 0);
    signal crc_check_en_s       : std_logic;
	
begin

    CRC32_gen_u : CRC32_gen
        port map (
            reset_i         => reset_i,
            clk_i           => clk_i,
            init_i          => init_i,
            data_i          => data_i,
            data_valid_i    => data_valid_i,                            
            CRC_o           => CRC_s
        );
    
    
    last_data_p : process(clk_i)
    begin
        if clk_i'event and clk_i = '1' then
            crc_check_en_s <= '0';
            if data_valid_i = '1' and data_last_i = '1' then
                crc_check_en_s <= '1';
            end if;
        end if;
    end process;
    
    crc_err_p : process(clk_i)
    begin 
        if clk_i'event and clk_i = '1' then
            crc_err_o <= '0';
            crc_err_valid_o <= '0';
            if crc_check_en_s = '1' then
                crc_err_valid_o <= '1';
                if CRC_s /= x"0000_0000" then
                    crc_err_o <= '1';
                end if;
            end if;
        end if;
    end process crc_err_p;
    
end rtl;

