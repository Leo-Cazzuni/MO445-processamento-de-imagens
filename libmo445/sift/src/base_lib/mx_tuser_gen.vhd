-------------------------------------------------------------------------------
--  Gera e verifica se os campos do tuser estao de acordo com o esperado.
-- Criado como gerador generico para todos os projetos.
-------------------------------------------------------------------------------
--Standard libraries
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
--
library base_lib;
use base_lib.base_lib_pkg.all;
-------------------------------------------------------------------------------
entity mx_tuser_gen is
    generic(
        -- Os limites sao configuraveis por generic, pois podem variar ao longo da cadeia de processamento.
        wcnt_limit_g    : integer := 1024;
        pcnt_limit_g    : integer := 32;
        bcnt_limit_g    : integer := 4;
        ocnt_limit_g    : integer := 29;
        -- Tamanhos dos contadores no tuser do projeto
        wcnt_size_g     : integer := vec_fit(1024);
        pcnt_size_g     : integer := vec_fit(27);
        bcnt_size_g     : integer := vec_fit(4);
        ocnt_size_g     : integer := vec_fit(29);
        tuser_bits_g    : integer := 128;
        -- Amostra ou nao os sinais de entrada.
        -- ATENCAO tuser_o NAO EH GERADO CORRETAMENTE COM sample_en_g = TRUE. A AMOSTRAGEM VALE APENAS PARA A VERIFICACAO DOS DADOS.
        sample_en_g     : boolean := false
    );
    port(
        arst_i          : in std_logic;
        aclk_i          : in std_logic;
        --
        wcnt_limit_i    : in  std_logic_vector(vec_fit(wcnt_limit_g) downto 0);
        pcnt_limit_i    : in  std_logic_vector(vec_fit(pcnt_limit_g) downto 0);
        --
        tvalid_i        : in  std_logic;
        tready_i        : in  std_logic;
        tlast_i         : in  std_logic;
        tuser_i         : in  std_logic_vector(tuser_bits_g-1 downto 0) := (others => '0');
        --
        tuser_o         : out std_logic_vector(tuser_bits_g-1 downto 0)
    );
end mx_tuser_gen;

architecture rtl of mx_tuser_gen is

    constant tuser_cnt_size_c   : integer := ocnt_size_g+bcnt_size_g+pcnt_size_g+wcnt_size_g;

    signal tvalid_s     : std_logic := '0';
    signal tready_s     : std_logic := '0';
    signal tlast_s      : std_logic := '0';
    signal tuser_s      : std_logic_vector(tuser_bits_g-1 downto 0) := (others => '0');
    signal tuser_o_s    : std_logic_vector(tuser_o'length-1 downto 0) := (others => '0');
    --
    signal wcnt_cnt     : std_logic_vector(wcnt_size_g-1 downto 0) := (others => '0');
    signal pcnt_cnt     : std_logic_vector(pcnt_size_g-1 downto 0) := (others => '0');
    signal bcnt_cnt     : std_logic_vector(bcnt_size_g-1 downto 0) := (others => '0');
    signal ocnt_cnt     : std_logic_vector(ocnt_size_g-1 downto 0) := (others => '0');
    --
    signal wcnt_last_s  : std_logic := '0';
    signal pcnt_last_s  : std_logic := '0';
    signal bcnt_last_s  : std_logic := '0';
    signal ocnt_last_s  : std_logic := '0';
    --
    signal wcnt_s       : std_logic_vector(wcnt_size_g-1 downto 0) := (others => '0');
    signal pcnt_s       : std_logic_vector(pcnt_size_g-1 downto 0) := (others => '0');
    signal bcnt_s       : std_logic_vector(bcnt_size_g-1 downto 0) := (others => '0');
    signal ocnt_s       : std_logic_vector(ocnt_size_g-1 downto 0) := (others => '0');
    
    signal wcnt_limit_2_s : unsigned(wcnt_limit_i'length-1 downto 0) := (others => '0');
    signal pcnt_limit_2_s : unsigned(pcnt_limit_i'length-1 downto 0) := (others => '0');

begin

    assert tuser_bits_g >= tuser_cnt_size_c report "TUSER deve ser maior ou igual ao tamanho dos contadores." severity failure;

-- Amostragem de entrada -----------------------------------------------------------------------------------------------
-- Pode ser bypassada por generic
    samp_on_gen: if sample_en_g = true generate
        samp_p: process(arst_i, aclk_i)
            begin
                if arst_i = '1' then
                    tvalid_s    <= '0';
                    tready_s    <= '0';
                    tlast_s     <= '0';
                    tuser_s     <= (others => '0');
                elsif aclk_i'event and aclk_i = '1' then
                    tvalid_s    <= tvalid_i;
                    tready_s    <= tready_i;
                    tlast_s     <= tlast_i;
                    tuser_s     <= tuser_i;
                end if;
            end process;
    end generate samp_on_gen;

    samp_off_gen: if sample_en_g = false generate
        tvalid_s    <= tvalid_i;
        tready_s    <= tready_i;
        tlast_s     <= tlast_i;
        tuser_s     <= tuser_i;
    end generate samp_off_gen;
------------------------------------------------------------------------------------------------------------------------
-- Contadores gerados internamente -------------------------------------------------------------------------------------
    wcnt_limit_p: process(aclk_i, arst_i)
    begin
        if arst_i = '1' then
            wcnt_limit_2_s <= (others => '0');
        elsif aclk_i'event and aclk_i = '1' then
            wcnt_limit_2_s <= unsigned(wcnt_limit_i) - 2;
        end if;
    end process;
    
    pcnt_limit_p: process(aclk_i, arst_i)
    begin
        if arst_i = '1' then
            pcnt_limit_2_s <= (others => '0');
        elsif aclk_i'event and aclk_i = '1' then
            pcnt_limit_2_s <= unsigned(pcnt_limit_i) - 2;
        end if;
    end process;

    wcnt_p: process(arst_i, aclk_i)
        begin
            if arst_i = '1' then
                wcnt_cnt <= (others => '0');
            elsif aclk_i'event and aclk_i = '1' then
                if tvalid_s = '1' and tready_s = '1' then
                    if wcnt_last_s = '1' then
                        wcnt_cnt <= (others => '0');
                    else
                        wcnt_cnt <= std_logic_vector(unsigned(wcnt_cnt) + 1);
                    end if;
                end if;
            end if;
        end process;

    wcnt_last_p: process(arst_i, aclk_i)
        begin
            if arst_i = '1' then
                wcnt_last_s <= '0';
            elsif aclk_i'event and aclk_i = '1' then
                if tvalid_s = '1' and tready_s = '1' then
                    if to_integer(unsigned(wcnt_cnt)) = wcnt_limit_2_s then
                        wcnt_last_s <= '1';
                    else
                        wcnt_last_s <= '0';
                    end if;
                end if;
            end if;
        end process;

    pcnt_p: process(arst_i, aclk_i)
        begin
            if arst_i = '1' then
                pcnt_cnt <= (others => '0');
            elsif aclk_i'event and aclk_i = '1' then
                if tvalid_s = '1' and tready_s = '1' and tlast_s = '1' then
                    if pcnt_last_s = '1' then
                        pcnt_cnt <= (others => '0');
                    else
                        pcnt_cnt <= std_logic_vector(unsigned(pcnt_cnt) + 1);
                    end if;
                end if;
            end if;
        end process;

    pcnt_last_p: process(arst_i, aclk_i)
        begin
            if arst_i = '1' then
                pcnt_last_s <= '0';
            elsif aclk_i'event and aclk_i = '1' then
                if tvalid_s = '1' and tready_s = '1' and tlast_s = '1' then
                    if to_integer(unsigned(pcnt_cnt)) = pcnt_limit_2_s then
                        pcnt_last_s <= '1';
                    else
                        pcnt_last_s <= '0';
                    end if;
                end if;
            end if;
        end process;

    bcnt_p: process(arst_i, aclk_i)
        begin
            if arst_i = '1' then
                bcnt_cnt <= (others => '0');
            elsif aclk_i'event and aclk_i = '1' then
                if tvalid_s = '1' and tready_s = '1' and tlast_s = '1' and pcnt_last_s = '1' then
                    if bcnt_last_s = '1' then
                        bcnt_cnt <= (others => '0');
                    else
                        bcnt_cnt <= std_logic_vector(unsigned(bcnt_cnt) + 1);
                    end if;
                end if;
            end if;
        end process;

    noburst_gen: if bcnt_limit_g <= 1 generate
        bcnt_last_s <= '1';
    end generate noburst_gen;

    burst_gen: if bcnt_limit_g > 1 generate
        bcnt_last_p: process(arst_i, aclk_i)
            begin
                if arst_i = '1' then
                    bcnt_last_s <= '0';
                elsif aclk_i'event and aclk_i = '1' then
                    if tvalid_s = '1' and tready_s = '1' and tlast_s = '1' and pcnt_last_s = '1' then
                        if to_integer(unsigned(bcnt_cnt)) = bcnt_limit_g-2 then
                            bcnt_last_s <= '1';
                        else
                            bcnt_last_s <= '0';
                        end if;
                    end if;
                end if;
            end process;
    end generate burst_gen;

    ocnt_p: process(arst_i, aclk_i)
        begin
            if arst_i = '1' then
                ocnt_cnt <= (others => '0');
            elsif aclk_i'event and aclk_i = '1' then
                if tvalid_s = '1' and tready_s = '1' and tlast_s = '1' and pcnt_last_s = '1' and bcnt_last_s = '1' then
                    if ocnt_last_s = '1' then
                        ocnt_cnt <= (others => '0');
                    else
                        ocnt_cnt <= std_logic_vector(unsigned(ocnt_cnt) + 1);
                    end if;
                end if;
            end if;
        end process;

    ocnt_last_p: process(arst_i, aclk_i)
        begin
            if arst_i = '1' then
                ocnt_last_s <= '0';
            elsif aclk_i'event and aclk_i = '1' then
                if tvalid_s = '1' and tready_s = '1' and tlast_s = '1' and pcnt_last_s = '1' and bcnt_last_s = '1' then
                    if to_integer(unsigned(ocnt_cnt)) = ocnt_limit_g-2 then
                        ocnt_last_s <= '1';
                    else
                        ocnt_last_s <= '0';
                    end if;
                end if;
            end if;
        end process;
    
    tuser_o_s(tuser_cnt_size_c-1 downto 0) <= ocnt_cnt & bcnt_cnt & pcnt_cnt & wcnt_cnt;
    
    tuser_o(tuser_bits_g-1 downto tuser_cnt_size_c) <= tuser_i(tuser_bits_g-1 downto tuser_cnt_size_c);
    tuser_o(tuser_cnt_size_c-1 downto 0)            <= tuser_o_s(tuser_cnt_size_c-1 downto 0);

end rtl;
