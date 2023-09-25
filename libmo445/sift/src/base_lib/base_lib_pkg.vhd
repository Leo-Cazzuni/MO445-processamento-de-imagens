------------------------------------------------------------------------------------------------------------------------
-- Basic Digital Systems Components Library
------------------------------------------------------------------------------------------------------------------------
use std.textio.all;
--Standard Libraries
library ieee;
use ieee.std_logic_1164.all;
--use ieee.std_logic_arith.all;
--use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;
use ieee.math_real.all;
------------------------------------------------------------------------------------------------------------------------
--   Basic Digital Systems Components Library
-- - Contains declarations and basic functions to more complex building blocks of programmable logic.
-- - Contains functions that implement asynchronous logic (similar to 7400 family)
------------------------------------------------------------------------------------------------------------------------
package base_lib_pkg is

    -- Usual vectors types
    type bit1vec_t  is array (natural range <>) of std_logic_vector( 0 downto 0);
    type bit2vec_t  is array (natural range <>) of std_logic_vector( 1 downto 0);
    type bit3vec_t  is array (natural range <>) of std_logic_vector( 2 downto 0);
    type nibble_t   is array (natural range <>) of std_logic_vector( 3 downto 0);
    type bit4vec_t  is array (natural range <>) of std_logic_vector( 4 downto 0);
    type bit5vec_t  is array (natural range <>) of std_logic_vector( 5 downto 0);
    type bit6vec_t  is array (natural range <>) of std_logic_vector( 6 downto 0);
    type byte_t     is array (natural range <>) of std_logic_vector( 7 downto 0);
    type bit9vec_t  is array (natural range <>) of std_logic_vector( 8 downto 0);
    type bit10vec_t is array (natural range <>) of std_logic_vector( 9 downto 0);
    type bit11vec_t is array (natural range <>) of std_logic_vector(10 downto 0);
    type bit12vec_t is array (natural range <>) of std_logic_vector(11 downto 0);
    type bit13vec_t is array (natural range <>) of std_logic_vector(12 downto 0);
    type bit14vec_t is array (natural range <>) of std_logic_vector(13 downto 0);
    type bit15vec_t is array (natural range <>) of std_logic_vector(14 downto 0);
    type word_t     is array (natural range <>) of std_logic_vector(15 downto 0);
    type bit17vec_t is array (natural range <>) of std_logic_vector(16 downto 0);
    type bit18vec_t is array (natural range <>) of std_logic_vector(17 downto 0);
    type bit19vec_t is array (natural range <>) of std_logic_vector(18 downto 0);
    type bit20vec_t is array (natural range <>) of std_logic_vector(19 downto 0);
    type bit21vec_t is array (natural range <>) of std_logic_vector(20 downto 0);
    type bit22vec_t is array (natural range <>) of std_logic_vector(21 downto 0);
    type bit23vec_t is array (natural range <>) of std_logic_vector(22 downto 0);
    type bit24vec_t is array (natural range <>) of std_logic_vector(23 downto 0);
    type bit25vec_t is array (natural range <>) of std_logic_vector(24 downto 0);
    type bit26vec_t is array (natural range <>) of std_logic_vector(25 downto 0);
    type bit27vec_t is array (natural range <>) of std_logic_vector(26 downto 0);
    type bit28vec_t is array (natural range <>) of std_logic_vector(27 downto 0);
    type bit29vec_t is array (natural range <>) of std_logic_vector(28 downto 0);
    type bit30vec_t is array (natural range <>) of std_logic_vector(29 downto 0);
    type bit31vec_t is array (natural range <>) of std_logic_vector(30 downto 0);
    type dword_t    is array (natural range <>) of std_logic_vector(31 downto 0);
    type qword_t    is array (natural range <>) of std_logic_vector(63 downto 0);
    type dqword_t   is array (natural range <>) of std_logic_vector(127 downto 0);
    type bit96vec_t is array (natural range <>) of std_logic_vector(95 downto 0);
    type bit192vec_t is array (natural range <>) of std_logic_vector(191 downto 0);
    type intvec_t   is array (natural range <>) of integer;
    type boolvec_t  is array (natural range <>) of boolean;

    -- Usual Unsigned types
    type ubyte_t    is array (natural range<>) of unsigned(7 downto 0);
    type udword_t   is array (natural range<>) of unsigned(31 downto 0);
    
    -- Functions
    function vec_fit        (constant input : in integer) return integer;
    function bin2gray       (signal input : in std_logic_vector) return std_logic_vector;
    function gray2bin       (signal input : in std_logic_vector) return std_logic_vector;
    function bin2onehot     (signal input : in std_logic_vector) return std_logic_vector;
    function onehot2bin     (signal input : in std_logic_vector) return std_logic_vector;
    function maj_vote       (signal input : in std_logic_vector) return std_logic;
    function or_vec_32b_f   (signal input : in dword_t) return std_logic_vector;
    function or_reduce_f    (signal input : in std_logic_vector) return std_logic ;
    function power2_f       (signal input : in std_logic_vector) return std_logic_vector;
    function and_reduce_f   (signal input : in std_logic_vector) return std_logic ;
    function maximum_f      (v : in intvec_t) return integer;
    function maximum_f      (a : integer; b : integer)  return integer;
    function minimum_f      (v : in intvec_t) return integer;
    function format_f       (arg : std_logic_vector; width : integer := 16)  return string;
    function format_f       (arg : intvec_t; width : integer := 1; cols  : integer := 16) return string;
    -- synthesis translate_off
    function format_f       (arg : integer; width : integer := 1)   return string;
    -- synthesis translate_on
    function zero_pad_f     (v : std_logic_vector; width : integer) return std_logic_vector;
    function zero_pad_f     (v : std_logic;        width : integer) return std_logic_vector;
    function swap_bits_f    (v : std_logic_vector) return std_logic_vector;
    function conv_std_logic (v : boolean) return std_logic;
    function count_ones_f   (input : in std_logic_vector) return std_logic_vector; -- Retorna o numero de 1's do vetor
    function total_f        (constant vec : in intvec_t) return integer;-- Soma cada valor de um vetor de inteiros
	function resize_sig_f   (signal input: std_logic_vector; constant size: integer) return std_logic_vector;
    function resize_unsig_f (signal input: std_logic_vector; constant size: integer) return std_logic_vector;
    function resize_csig_f  (signal input: std_logic_vector; constant size: integer) return std_logic_vector;
    function resize_cunsig_f(signal input: std_logic_vector; constant size: integer) return std_logic_vector;
    function get_signal_f   (signal input: std_logic_vector; constant size: integer; constant start: integer) return unsigned;
	---------------------------------------------------------------------------
    -- O tamanho do barramento de dados do MIG é configurável
    ---------------------------------------------------------------------------
    constant MIG_DWIDTH_C : integer := 32;

    ---------------------------------------------------------------------------
    -- Tipos de dados da interface AXI utilizado no MIG
    ---------------------------------------------------------------------------
    -- O sinal Tready tem sentido contrário por isso será utilizado o próprio sinal na instanciação
    type axi_mig_t is record
        tvalid : std_logic;
        tlast  : std_logic;
        tdata  : std_logic_vector(MIG_DWIDTH_C-1 downto 0);
        tuser  : std_logic_vector(31 downto 0);
        tkeep  : std_logic_vector(3 downto 0);
    end record;

    -- Essa constante pode ser utilizada para iniciar algum objeto do axi_mig_t
    constant null_axi_mig_c : axi_mig_t := (tvalid => '0', tlast => '0', tdata => (others => '0'),
                                            tuser => (others => '0'), tkeep => (others => '0'));

    type vec_mig_t is array (natural range <>) of axi_mig_t;

    -- Interface AXI para 64bits na porta de dados
    type axi64_mig_t is record
        tvalid : std_logic;
        tlast  : std_logic;
        tdata  : std_logic_vector(MIG_DWIDTH_C*2-1 downto 0);
        tuser  : std_logic_vector(31 downto 0);
        tkeep  : std_logic_vector(7 downto 0);
    end record;

    -- Essa constante pode ser utilizada para iniciar algum objeto do axi_mig_t
    constant null_axi64_mig_c : axi64_mig_t := (tvalid => '0', tlast => '0', tdata => (others => '0'),
                                                tuser => (others => '0'), tkeep => (others => '0'));

    type vec_mig_64_t is array (natural range <>) of axi64_mig_t;

    type srf_read_t is record
        tvalid : std_logic;
        tlast  : std_logic;
        tdata  : std_logic_vector(127 downto 0);
        tuser  : std_logic_vector(95 downto 0);
        tid    : std_logic_vector(255 downto 0);
        tkeep  : std_logic_vector(15 downto 0);
        tready : std_logic;
    end record;

    type vec_srf_read_t is array (natural range <>) of srf_read_t;


    --Generic ram, dual port
    component generic_ram
        Generic (
            addr_size_c         : integer := 8;
            port_size_c         : integer := 8;
            output_sync         : boolean := false;
            ram_type            : string  := "distributed";
            DOUBLE_SAMP_EN_G    : boolean := false
        );
        Port (
            rsta_i   : in  std_logic;
            clka_i   : in  std_logic;
            wrena_i  : in  std_logic;
            ena_i    : in  std_logic;
            dataa_i  : in  std_logic_vector(port_size_c-1 downto 0);
            dataa_o  : out std_logic_vector(port_size_c-1 downto 0);
            addra_i  : in  std_logic_vector(addr_size_c-1 downto 0);
            rstb_i   : in  std_logic;
            clkb_i   : in  std_logic;
            enb_i    : in  std_logic;
            datab_o  : out std_logic_vector(port_size_c-1 downto 0);
            addrb_i  : in  std_logic_vector(addr_size_c-1 downto 0)
        );
    end component;

    --Bit ram, dual port
    component bit_ram
        Generic (
            addr_size_c : integer := 8;
            output_sync : boolean := false;
            ram_type    : string  := "distributed"
        );
        Port (
            rsta_i   : in  std_logic;
            clka_i   : in  std_logic;
            wrena_i  : in  std_logic;
            ena_i    : in  std_logic;
            dataa_i  : in  std_logic;
            dataa_o  : out std_logic;
            addra_i  : in  std_logic_vector(addr_size_c-1 downto 0);
            rstb_i   : in  std_logic;
            clkb_i   : in  std_logic;
            enb_i    : in  std_logic;
            datab_o  : out std_logic;
            addrb_i  : in  std_logic_vector(addr_size_c-1 downto 0)
        );
    end component;

    --Generic ram, true dual port
    component true_dualport_ram
        Generic (
            addr_size_c : integer := 8;
            port_size_c : integer := 8
        );
        Port (
            rsta_i   : in  std_logic;
            clka_i   : in  std_logic;
            wrena_i  : in  std_logic;
            rdena_i  : in  std_logic;
            ena_i    : in  std_logic;
            dataa_i  : in  std_logic_vector(port_size_c-1 downto 0);
            dataa_o  : out std_logic_vector(port_size_c-1 downto 0);
            addra_i  : in  std_logic_vector(addr_size_c-1 downto 0);
            rstb_i   : in  std_logic;
            clkb_i   : in  std_logic;
            wrenb_i  : in  std_logic;
            rdenb_i  : in  std_logic;
            enb_i    : in  std_logic;
            datab_i  : in  std_logic_vector(port_size_c-1 downto 0);
            datab_o  : out std_logic_vector(port_size_c-1 downto 0);
            addrb_i  : in  std_logic_vector(addr_size_c-1 downto 0)
        );
    end component;

    component true_dualportbe_ram
        Port (
           --Port A
           rsta_i    : in  std_logic;
           clka_i    : in  std_logic;
           wrena_i   : in  std_logic_vector(3 downto 0);
           rdena_i   : in  std_logic;
           dataa_i   : in  std_logic_vector(31 downto 0);
           dataa_o   : out std_logic_vector(31 downto 0);
           addra_i   : in  std_logic_vector(9 downto 0);
           --Port B
           rstb_i    : in  std_logic;
           clkb_i    : in  std_logic;
           wrenb_i   : in  std_logic_vector(3 downto 0);
           rdenb_i   : in  std_logic;
           datab_i   : in  std_logic_vector(31 downto 0);
           datab_o   : out std_logic_vector(31 downto 0);
           addrb_i   : in  std_logic_vector(9 downto 0)
        );
    end component;

    component dco
        Generic (
            K_c         : integer := 8
        );
        Port (
            clk_i       : in  std_logic;
            resync_i    : in  std_logic;
            prescale_i  : in  std_logic;
            N_i         : in  std_logic_vector(K_c-1 downto 0);
            clk_o       : out std_logic
        );
    end component;

    component decimation
        generic(
            datasize             :  integer := 14;  --bit vector size of input data port.
            decimationsize       :  integer := 16   --bit vector size of decimation.
        );
        port(
            clk_i                :  in std_logic;   --clock.
            rst_i                :  in std_logic;   --active-high reset.

            data_a_i             :  in std_logic_vector(datasize-1 downto 0);        --input data a.
            data_b_i             :  in std_logic_vector(datasize-1 downto 0);        --input data b.
            ovr_a_i              :  in std_logic;                                    --overange input a.
            ovr_b_i              :  in std_logic;                                    --overange input b.
            gate_i               :  in std_logic;                                    --enable the data aquisition in the block.

            decimation_i         :  in std_logic_vector(decimationsize-1 downto 0);  --decimation reduces the original sampling rate for a sequence to a lower rate.

            data_a_o             :  out std_logic_vector(datasize-1 downto 0);       --output data a.
            data_b_o             :  out std_logic_vector(datasize-1 downto 0);       --output data b port only used when decimation has factor 1.
            ovr_a_o              :  out std_logic;                                   --overange output a.
            ovr_b_o              :  out std_logic;                                   --overange output b.
            data_valid_a_o       :  out std_logic;                                   --data valid a at output.
            data_valid_b_o       :  out std_logic                                    --data valid b at output.
        );
    end component;

    component CRC32_gen
        port
        (
            reset_i         : in std_logic;
            clk_i           : in std_logic;
            init_i          : in std_logic;
            data_i          : in std_logic_vector(31 downto 0);
            data_valid_i    : in std_logic;

            CRC_o           : out std_logic_vector(31 downto 0)
        );

    end component;

    component CRC32_check
        port
        (
            reset_i         : in std_logic;
            clk_i           : in std_logic;
            init_i          : in std_logic; -- Pulso de início, deve ser pulsado antes do início da recepção de cada pacote.

            data_i          : in std_logic_vector(31 downto 0);
            data_valid_i    : in std_logic; -- Indica se o dado é válido.

            data_last_i     : in std_logic; -- Indica se o dado recebido é o último dado do pacote, ou seja,
                                            -- se o dado corresponde ao CRC do frame a ser checado.

            crc_err_valid_o : out std_logic;-- Indica se o sinal crc_err_o é válido, ele é setado no
                                            -- segundo ciclo após a recepção do último dado do pacote.
            crc_err_o       : out std_logic -- Quando 1, ele representa um erro de checagem do CRC.
        );
    end component;

    -- This is ugly but works...
    constant IS_SIMULATION_C : boolean :=
        False
        -- synthesis translate_off
        or True
        -- synthesis translate_on
        ;

end base_lib_pkg;

package body base_lib_pkg is

    -- Calculates the vector needed to represent the number
    -- - input:   integer we want to vectorize (must be positive)
    -- - output:  N (vector size)
    -- - What is done:
    --   - N = LOG2(input)
    --   - Round up only.
    function vec_fit    (constant input : in integer) return integer is
        variable tmp : integer;
        begin
            tmp := 1;
				if 2**tmp < input then
					while 2**tmp < input loop
						tmp := tmp + 1;
					end loop;
				end if;
            return tmp;
    end vec_fit;

    --Binary to gray converter
    -- - STD_LOGIC_VECTOR input
    -- - STD_LOGIC_VECTOR output
    function bin2gray (signal input : in std_logic_vector) return std_logic_vector is
        variable tmp : std_logic_vector(input'range);
        begin
            for j in input'range loop
                if j = input'high then
                    tmp(j) := input(j);
                else
                    tmp(j) := input(j) xor input(j+1);
                end if;
            end loop;
            return tmp;
    end bin2gray;

    --Gray to binary converter
    -- - STD_LOGIC_VECTOR input
    -- - STD_LOGIC_VECTOR output
    function gray2bin (signal input : in std_logic_vector) return std_logic_vector is
        variable tmp : std_logic_vector(input'range);
        begin
            for j in input'range loop
                if j = input'high then
                    tmp(j) := input(j);
                else
                    tmp(j) := input(j) xor tmp(j+1);
                end if;
            end loop;
            return tmp;
    end gray2bin;

    --Binary to one-hot converter
    -- - STD_LOGIC_VECTOR input
    -- - STD_LOGIC_VECTOR output
    -- - Note: output will be a vector with F$ 2^N F$ size (it increases exponentially, so be careful)
    function bin2onehot (signal input : in std_logic_vector) return std_logic_vector is
        variable tmp : std_logic_vector(2**(input'length)-1 downto 0);
        begin
            tmp := (others=>'0');
            for j in 2**input'length-1 downto 0 loop
                if j = to_integer(unsigned(input)) then
                    tmp(j) := '1';
                    exit;
                end if;
            end loop;
            return tmp;
    end bin2onehot;

    --One-hot to binary converter
    -- - STD_LOGIC_VECTOR input
    -- - STD_LOGIC_VECTOR output
    -- - Note: will be LOG2(input vector size)
    function onehot2bin (signal input : in std_logic_vector) return std_logic_vector is
        constant tmp_size_c : integer := vec_fit(input'length);
        variable tmp : std_logic_vector(tmp_size_c-1 downto 0);
        begin
            tmp := (others=>'0');
            for j in input'length-1 downto 0 loop
                if input(j) = '1' then
                    tmp := std_logic_vector(to_unsigned(j, tmp_size_c));
                    exit;
                end if;
            end loop;
            return tmp;
    end onehot2bin;

    --Majority Vote function
    -- - Selects the most common value on its input (0 or 1).
    -- - In case of tie, output will favor '1'.
    function maj_vote (signal input : in std_logic_vector) return std_logic is
        variable tmp : integer;
        begin
            tmp := 0;
            for j in input'range loop
                tmp := tmp + to_integer(unsigned(input(j downto j)));
            end loop;
            if tmp >= input'length / 2 then
                return '1';
            else
                return '0';
            end if;
    end maj_vote;

    --Faz or bit a bit de um vetor de n posições 32 bits.
    function or_vec_32b_f (signal input : in dword_t) return std_logic_vector is
        variable temp_v : std_logic_vector(31 downto 0);
        begin
            temp_v := (others => '0');
            for j in input'range loop
                temp_v := temp_v or input(j);
            end loop;
            return temp_v;
        end or_vec_32b_f;

    --Faz or bit a bit de um vetor de tamanho variável
    function or_reduce_f (signal input : in std_logic_vector) return std_logic is
        variable temp_v : std_logic := '0';
        begin
            temp_v := '0';
            for j in input'range loop
                temp_v := temp_v or input(j);
            end loop;
            return temp_v;
        end or_reduce_f;

-- função para transformar um número qualquer no complemento de 2 imediatamente menor.
-- se o valor de entrada for 0, a função deve retornar 1.
    function power2_f (signal input : in std_logic_vector) return std_logic_vector is
        variable power2_v : std_logic_vector(input'range);
        variable found1_v : std_logic;
        begin
            power2_v := (0 => '1', others => '0');
            found1_v := '0';
            for j in input'high downto 0 loop
                if input(j) = '1' and found1_v = '0' then
                    power2_v(j) := '1';
                    found1_v    := '1';
                elsif found1_v = '1' then
                    power2_v(j) := '0';
                end if;
            end loop;
            return power2_v;
    end power2_f;
        --Faz and bit a bit de um vetor de tamanho variável
    function and_reduce_f (signal input : in std_logic_vector) return std_logic is
        variable temp_v : std_logic := '0';
        begin
            temp_v := '1';
            for j in input'range loop
                temp_v := temp_v and input(j);
            end loop;
            return temp_v;
        end and_reduce_f;

    function maximum_f (v : in intvec_t) return integer is
        variable result : integer;
    begin
        result := 0;
        for i in v'range loop
            if v(i) > result then
                result := v(i);
            end if;
        end loop;
        return result;
    end maximum_f;

    ----------------------------------------------------------------------------
    -- Returns the maximum value between 2 numbers
    ----------------------------------------------------------------------------
    function maximum_f (a : integer; b : integer) return integer is
    begin
        if a > b then
            return a;
        else
            return b;
        end if;
    end maximum_f;

    function minimum_f (v : in intvec_t) return integer is
        variable result : integer;
    begin
        result := 0;
        for i in v'range loop
            if result = 0 then
                result := v(i);
            elsif v(i) < result then
                result := v(i);
            end if;
        end loop;
        return result;
    end minimum_f;

    ----------------------------------------------------------------------------
    -- Format a std_logic_vector as ASCII sequence for printing
    ----------------------------------------------------------------------------
    function format_f (
        constant arg             : std_logic_vector;
        constant width           : integer := 16) return string is
        variable input_width     : integer := maximum_f(arg'high - arg'low + 1, width);
        variable calc_width      : integer := 4*((input_width - 1)/4 + 1);
        variable vector_adjusted : std_logic_vector(calc_width - 1 downto 0);
        variable result          : string(calc_width/4 downto 1) := (others => ' ');

        ----------------------------------------------------------------------------
        -- Converts a nibble to ASCII for debugging purposes
        ----------------------------------------------------------------------------
        function to_char_f  (
            constant arg          : std_logic_vector(3 downto 0)) return character is
            variable vector_adjusted : std_logic_vector(3 downto 0) := (others => '0');
            variable result          : character;
        begin
            for i in arg'range loop
                if arg(i) = '1' or arg(i) = 'H' then
                    vector_adjusted(i) := '1';
                else
                    vector_adjusted(i)  := '0';
                end if;
            end loop;

            case vector_adjusted is
                when x"0" => result := '0';
                when x"1" => result := '1';
                when x"2" => result := '2';
                when x"3" => result := '3';
                when x"4" => result := '4';
                when x"5" => result := '5';
                when x"6" => result := '6';
                when x"7" => result := '7';
                when x"8" => result := '8';
                when x"9" => result := '9';
                when x"A" => result := 'A';
                when x"B" => result := 'B';
                when x"C" => result := 'C';
                when x"D" => result := 'D';
                when x"E" => result := 'E';
                when x"F" => result := 'F';
                when others => result := 'X';
            end case;

            return result;

        end to_char_f;

    begin
        if arg'ascending then
            for i in arg'high downto arg'low loop
                vector_adjusted(i)   := arg(arg'high - i);
            end loop;
        else
            vector_adjusted(arg'high downto arg'low) := arg(arg'high downto arg'low);
        end if;

        for i in (calc_width/4) - 1 downto 0 loop
            result(i + 1) := to_char_f(vector_adjusted(4*(i+1) - 1 downto 4*i));
        end loop;
        return result;
    end format_f;

    ----------------------------------------------------------------------------
    -- Format an ASCII sequence for printing
    ----------------------------------------------------------------------------
    function format_f (
            constant arg   : integer;
            constant width : integer := 1) return string is
            -- synthesis translate_off
            variable L     : line;
            -- synthesis translate_on
        begin
            -- synthesis translate_off

            for i in 0 to width - integer'image(arg)'length - 1 loop
                write(L, string'(" "));
            end loop;
            write(L, integer'image(arg));
            return L.all;
            -- synthesis translate_on
            return integer'image(arg);
    end format_f;

    ----------------------------------------------------------------------------
    -- Format an intvec_t as an ASCII table for printing
    ----------------------------------------------------------------------------
    function format_f(
        constant arg        : intvec_t;
        constant width      : integer := 1;
        constant cols       : integer := 16) return string is
        variable L          : line;
        variable char_width : integer := integer'image(maximum_f(arg))'length;
    begin
        char_width := maximum_f(width, char_width);
        for i in 0 to arg'length - 1 loop
            write(L, format_f(arg(i), char_width) & " ");
            if (i + 1) mod cols = 0 then
                write(L, cr);
            end if;
        end loop;

        return L.all;
    end function;

    function swap_bits_f ( v : std_logic_vector) return std_logic_vector is
        variable result : std_logic_vector(v'range);
        begin
            for i in v'range loop
                result(v'high - i + v'low) := v(i);
            end loop;
            return result;
        end function swap_bits_f;

    function zero_pad_f (v : std_logic_vector; width : integer) return std_logic_vector is
        variable result : std_logic_vector(width - 1 downto 0);
    begin
        result  := (others => '0');
        result(v'range) := v;
        return result;
    end function zero_pad_f;

    function zero_pad_f (v : std_logic; width : integer) return std_logic_vector is
        variable result : std_logic_vector(width - 1 downto 0);
    begin
        result    := (others => '0');
        result(0) := v;
        return result;
    end function zero_pad_f;
    function conv_std_logic (v      : boolean) return std_logic is
    begin
        if v then
            return '1';
        else
            return '0';
        end if;
    end function conv_std_logic;

    -- Retorna o numero de 1's do vetor
    function count_ones_f (input : in std_logic_vector) return std_logic_vector is
        variable result_v : std_logic_vector(vec_fit(input'length) downto 0);
    begin
        result_v := (others => '0');
        for j in input'range loop
            if input(j) = '1' then
                result_v := std_logic_vector(unsigned(result_v) + 1);
            end if;
        end loop;
        return result_v;
    end function count_ones_f;

    -- Soma cada valor de um vetor de inteiros
    function total_f (constant vec : in intvec_t) return integer is
        variable total_out : integer := 0;
        begin
            for j in vec'range loop
                total_out := total_out + vec(j);
            end loop;
            return total_out;
        end total_f;

	-- Faz o resize de um sinal "signed"
    function resize_sig_f (signal input: in  std_logic_vector; constant size: integer) return std_logic_vector is
        variable result_v : std_logic_vector(size-1 downto 0) := (others => '0');
    begin
        result_v := std_logic_vector(resize(signed(input),size));
        return result_v;
    end function resize_sig_f;
    
    -- Faz o resize de um sinal "unsigned"
    function resize_unsig_f (signal input: in  std_logic_vector; constant size: integer) return std_logic_vector is
        variable result_v : std_logic_vector(size-1 downto 0) := (others => '0');
    begin
        result_v := std_logic_vector(resize(unsigned(input),size));
        return result_v;
    end function resize_unsig_f;
    
    -- Faz o resize de um sinal complex "signed"
    function resize_csig_f (signal input: in  std_logic_vector; constant size: integer) return std_logic_vector is
        variable result_v : std_logic_vector(size-1 downto 0) := (others => '0');
        variable re_v     : std_logic_vector(size/2-1 downto 0) := (others => '0');
        variable im_v     : std_logic_vector(size/2-1 downto 0) := (others => '0');
    begin
        re_v     := std_logic_vector(resize(signed(input(input'length/2-1 downto 0)),size/2));
        im_v     := std_logic_vector(resize(signed(input(input'length-1 downto input'length/2)),size/2));
        result_v := im_v & re_v;
        return result_v;
    end function resize_csig_f;
    
    -- Faz o resize de um sinal complex "unsigned"
    function resize_cunsig_f (signal input: in  std_logic_vector; constant size: integer) return std_logic_vector is
        variable result_v : std_logic_vector(size-1 downto 0) := (others => '0');
        variable re_v     : std_logic_vector(size/2-1 downto 0) := (others => '0');
        variable im_v     : std_logic_vector(size/2-1 downto 0) := (others => '0');
    begin
        re_v     := std_logic_vector(resize(signed(input(input'length/2-1 downto 0)),size/2));
        im_v     := std_logic_vector(resize(signed(input(input'length-1 downto input'length/2)),size/2));
        result_v := im_v & re_v;
        return result_v;
    end function resize_cunsig_f;
    
    -- Extraia uma parte do sinal de entrada, passando o tamanho da extracao e bit de inicio
    function get_signal_f (signal input: std_logic_vector; constant size: integer; constant start: integer) return unsigned is
        variable result_v : unsigned(size-1 downto 0) := (others => '0');
    begin
        result_v := unsigned(input(size+start-1 downto start));
        return result_v;
    end function get_signal_f;

end base_lib_pkg;
