------------------------------------------------------------------------------------------------------------------------
-- Testbench for the base_lib functions.
------------------------------------------------------------------------------------------------------------------------
-- IEEE standard library
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
-- Base library
library base_lib;
use base_lib.base_lib_pkg.all;

entity base_func_TB is
end base_func_TB;

architecture behavioral of base_func_TB is

-- vec_fit ---------------------------------------------------------------------
    type vec_int_t is array (natural range <>) of integer;
-- Input values for the vec_fit function.
    constant vec_value_c        : vec_int_t(7 downto 0) := (
        7   => 16,
        6   => 17,
        5   => 30,
        4   => 0,
        3   => 1,
        2   => 2,
        1   => 23,
        0   => 100
    );
-- The constant is assigned to a signal to help the visualization.
    signal vec_value_s          : vec_int_t(7 downto 0) := vec_value_c;
-- Sinal for the vec_fit results.
    signal vec_size_s           : vec_int_t(7 downto 0);
-- vec_fit expected values.
    constant expected_size_c    : vec_int_t(7 downto 0) := (
        7   => 4,
        6   => 5,
        5   => 5,
        4   => 1,
        3   => 1,
        2   => 1,
        1   => 5,
        0   => 7
    );
    signal vec_fit_err_s        : std_logic;

-- bin2gray / gray2bin ---------------------------------------------------------
    type vec_2bit_t is array (natural range <>) of std_logic_vector(1 downto 0);
    type vec_3bit_t is array (natural range <>) of std_logic_vector(2 downto 0);
    type vec_4bit_t is array (natural range <>) of std_logic_vector(3 downto 0);
-- Correct values for the 2 bit binary and gray codes.
    signal bin_2bit_s           : vec_2bit_t(3 downto 0) := (
        3   => "11",
        2   => "10",
        1   => "01",
        0   => "00"
    );
    signal gray_2bit_s          : vec_2bit_t(3 downto 0) := (
        3   => "10",
        2   => "11",
        1   => "01",
        0   => "00"
    );
-- Conversion output.
    signal bin2gray_2bit_s      : vec_2bit_t(3 downto 0);
    signal gray2bin_2bit_s      : vec_2bit_t(3 downto 0);
-- Error.
    signal bin2gray_2bit_err_s  : std_logic;
    signal gray2bin_2bit_err_s  : std_logic;

-- Correct values for the 3 bit binary and gray codes.
    signal bin_3bit_s           : vec_3bit_t(7 downto 0) := (
        7   => "111",
        6   => "110",
        5   => "101",
        4   => "100",
        3   => "011",
        2   => "010",
        1   => "001",
        0   => "000"
    );
    signal gray_3bit_s          : vec_3bit_t(7 downto 0) := (
        7   => "100",
        6   => "101",
        5   => "111",
        4   => "110",
        3   => "010",
        2   => "011",
        1   => "001",
        0   => "000"
    );
-- Conversion output.
    signal bin2gray_3bit_s      : vec_3bit_t(7 downto 0);
    signal gray2bin_3bit_s      : vec_3bit_t(7 downto 0);
-- Error.
    signal bin2gray_3bit_err_s  : std_logic;
    signal gray2bin_3bit_err_s  : std_logic;

-- Correct values for the 4 bit binary and gray codes.
    signal bin_4bit_s           : vec_4bit_t(15 downto 0) := (
        15  => "1111",
        14  => "1110",
        13  => "1101",
        12  => "1100",
        11  => "1011",
        10  => "1010",
        9   => "1001",
        8   => "1000",
        7   => "0111",
        6   => "0110",
        5   => "0101",
        4   => "0100",
        3   => "0011",
        2   => "0010",
        1   => "0001",
        0   => "0000"
    );
    signal gray_4bit_s          : vec_4bit_t(15 downto 0) := (
        15  => "1000",
        14  => "1001",
        13  => "1011",
        12  => "1010",
        11  => "1110",
        10  => "1111",
        9   => "1101",
        8   => "1100",
        7   => "0100",
        6   => "0101",
        5   => "0111",
        4   => "0110",
        3   => "0010",
        2   => "0011",
        1   => "0001",
        0   => "0000"
    );
-- Conversion output.
    signal bin2gray_4bit_s      : vec_4bit_t(15 downto 0);
    signal gray2bin_4bit_s      : vec_4bit_t(15 downto 0);
-- Error.
    signal bin2gray_4bit_err_s  : std_logic;
    signal gray2bin_4bit_err_s  : std_logic;

-- bin2onehot / onehot2bin -----------------------------------------------------
    signal onehot_s             : vec_4bit_t(3 downto 0);
    signal onehot_expected_s    : vec_4bit_t(3 downto 0) := (
        3   => "1000",
        2   => "0100",
        1   => "0010",
        0   => "0001"
    );
    signal onehot_err_s         : std_logic;
    signal bin_s                : vec_2bit_t(7 downto 0);
    signal bin_expected_s       : vec_2bit_t(7 downto 0) := (
        7   => "10",
        6   => "10",
        5   => "10",
        4   => "10",
        3   => "01",
        2   => "01",
        1   => "00",
        0   => "00"
    );
    signal bin_err_s            : std_logic;

-- maj_vote --------------------------------------------------------------------
    signal maj_vote_s           : std_logic_vector(15 downto 0);
    signal maj_vote_expected_s  : std_logic_vector(15 downto 0) := (
        15  => '1',
        14  => '1',
        13  => '1',
        12  => '1',
        11  => '1',
        10  => '1',
        9   => '1',
        8   => '0',
        7   => '1',
        6   => '1',
        5   => '1',
        4   => '0',
        3   => '1',
        2   => '0',
        1   => '0',
        0   => '0'
    );
    signal maj_vote_err_s       : std_logic;
--------------------------------------------------------------------------------

begin

-- vec_fit ---------------------------------------------------------------------
    vec_fit_gen: for j in 7 downto 0 generate

        vec_size_s(j) <= vec_fit(vec_value_c(j));

    end generate;

    vec_fit_err_s <= '0' when vec_size_s = expected_size_c else '1';
--------------------------------------------------------------------------------
-- bin2gray --------------------------------------------------------------------
    bin2gray_2bit_gen: for j in 3 downto 0 generate

        bin2gray_2bit_s(j) <= bin2gray(bin_2bit_s(j));
        gray2bin_2bit_s(j) <= gray2bin(gray_2bit_s(j));

    end generate;

    bin2gray_2bit_err_s <= '0' when bin2gray_2bit_s = gray_2bit_s else '1';
    gray2bin_2bit_err_s <= '0' when gray2bin_2bit_s = bin_2bit_s else '1';

    bin2gray_3bit_gen: for j in 7 downto 0 generate

        bin2gray_3bit_s(j) <= bin2gray(bin_3bit_s(j));
        gray2bin_3bit_s(j) <= gray2bin(gray_3bit_s(j));

    end generate;

    bin2gray_3bit_err_s <= '0' when bin2gray_3bit_s = gray_3bit_s else '1';
    gray2bin_3bit_err_s <= '0' when gray2bin_3bit_s = bin_3bit_s else '1';

    bin2gray_4bit_gen: for j in 15 downto 0 generate

        bin2gray_4bit_s(j) <= bin2gray(bin_4bit_s(j));
        gray2bin_4bit_s(j) <= gray2bin(gray_4bit_s(j));

    end generate;

    bin2gray_4bit_err_s <= '0' when bin2gray_4bit_s = gray_4bit_s else '1';
    gray2bin_4bit_err_s <= '0' when gray2bin_4bit_s = bin_4bit_s else '1';
--------------------------------------------------------------------------------
-- bin2onehot / onehot2bin -----------------------------------------------------

    bin2onehot_gen: for j in 3 downto 0 generate

        onehot_s(j) <= bin2onehot(bin_2bit_s(j));

    end generate;

    onehot_err_s <= '0' when onehot_s = onehot_expected_s else '1';

    onehot2bin_gen: for j in 7 downto 0 generate

        bin_s(j) <= onehot2bin(bin_3bit_s(j));

    end generate;

    bin_err_s <= '0' when bin_s = bin_expected_s else '1';

--------------------------------------------------------------------------------
-- maj_vote --------------------------------------------------------------------

    maj_vote_gen: for j in 15 downto 0 generate

        maj_vote_s(j) <= maj_vote(bin_4bit_s(j));

    end generate;

    maj_vote_err_s <= '0' when maj_vote_s = maj_vote_expected_s else '1';

--------------------------------------------------------------------------------
end behavioral;