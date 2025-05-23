	component soc_system is
		port (
			audio_0_external_interface_BCLK                  : in    std_logic                     := 'X';             -- BCLK
			audio_0_external_interface_DACDAT                : out   std_logic;                                        -- DACDAT
			audio_0_external_interface_DACLRCK               : in    std_logic                     := 'X';             -- DACLRCK
			audio_and_video_config_0_external_interface_SDAT : inout std_logic                     := 'X';             -- SDAT
			audio_and_video_config_0_external_interface_SCLK : out   std_logic;                                        -- SCLK
			audio_pll_0_audio_clk_clk                        : out   std_logic;                                        -- clk
			clk_clk                                          : in    std_logic                     := 'X';             -- clk
			hps_hps_io_emac1_inst_TX_CLK                     : out   std_logic;                                        -- hps_io_emac1_inst_TX_CLK
			hps_hps_io_emac1_inst_TXD0                       : out   std_logic;                                        -- hps_io_emac1_inst_TXD0
			hps_hps_io_emac1_inst_TXD1                       : out   std_logic;                                        -- hps_io_emac1_inst_TXD1
			hps_hps_io_emac1_inst_TXD2                       : out   std_logic;                                        -- hps_io_emac1_inst_TXD2
			hps_hps_io_emac1_inst_TXD3                       : out   std_logic;                                        -- hps_io_emac1_inst_TXD3
			hps_hps_io_emac1_inst_RXD0                       : in    std_logic                     := 'X';             -- hps_io_emac1_inst_RXD0
			hps_hps_io_emac1_inst_MDIO                       : inout std_logic                     := 'X';             -- hps_io_emac1_inst_MDIO
			hps_hps_io_emac1_inst_MDC                        : out   std_logic;                                        -- hps_io_emac1_inst_MDC
			hps_hps_io_emac1_inst_RX_CTL                     : in    std_logic                     := 'X';             -- hps_io_emac1_inst_RX_CTL
			hps_hps_io_emac1_inst_TX_CTL                     : out   std_logic;                                        -- hps_io_emac1_inst_TX_CTL
			hps_hps_io_emac1_inst_RX_CLK                     : in    std_logic                     := 'X';             -- hps_io_emac1_inst_RX_CLK
			hps_hps_io_emac1_inst_RXD1                       : in    std_logic                     := 'X';             -- hps_io_emac1_inst_RXD1
			hps_hps_io_emac1_inst_RXD2                       : in    std_logic                     := 'X';             -- hps_io_emac1_inst_RXD2
			hps_hps_io_emac1_inst_RXD3                       : in    std_logic                     := 'X';             -- hps_io_emac1_inst_RXD3
			hps_hps_io_sdio_inst_CMD                         : inout std_logic                     := 'X';             -- hps_io_sdio_inst_CMD
			hps_hps_io_sdio_inst_D0                          : inout std_logic                     := 'X';             -- hps_io_sdio_inst_D0
			hps_hps_io_sdio_inst_D1                          : inout std_logic                     := 'X';             -- hps_io_sdio_inst_D1
			hps_hps_io_sdio_inst_CLK                         : out   std_logic;                                        -- hps_io_sdio_inst_CLK
			hps_hps_io_sdio_inst_D2                          : inout std_logic                     := 'X';             -- hps_io_sdio_inst_D2
			hps_hps_io_sdio_inst_D3                          : inout std_logic                     := 'X';             -- hps_io_sdio_inst_D3
			hps_hps_io_usb1_inst_D0                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D0
			hps_hps_io_usb1_inst_D1                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D1
			hps_hps_io_usb1_inst_D2                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D2
			hps_hps_io_usb1_inst_D3                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D3
			hps_hps_io_usb1_inst_D4                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D4
			hps_hps_io_usb1_inst_D5                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D5
			hps_hps_io_usb1_inst_D6                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D6
			hps_hps_io_usb1_inst_D7                          : inout std_logic                     := 'X';             -- hps_io_usb1_inst_D7
			hps_hps_io_usb1_inst_CLK                         : in    std_logic                     := 'X';             -- hps_io_usb1_inst_CLK
			hps_hps_io_usb1_inst_STP                         : out   std_logic;                                        -- hps_io_usb1_inst_STP
			hps_hps_io_usb1_inst_DIR                         : in    std_logic                     := 'X';             -- hps_io_usb1_inst_DIR
			hps_hps_io_usb1_inst_NXT                         : in    std_logic                     := 'X';             -- hps_io_usb1_inst_NXT
			hps_hps_io_spim1_inst_CLK                        : out   std_logic;                                        -- hps_io_spim1_inst_CLK
			hps_hps_io_spim1_inst_MOSI                       : out   std_logic;                                        -- hps_io_spim1_inst_MOSI
			hps_hps_io_spim1_inst_MISO                       : in    std_logic                     := 'X';             -- hps_io_spim1_inst_MISO
			hps_hps_io_spim1_inst_SS0                        : out   std_logic;                                        -- hps_io_spim1_inst_SS0
			hps_hps_io_uart0_inst_RX                         : in    std_logic                     := 'X';             -- hps_io_uart0_inst_RX
			hps_hps_io_uart0_inst_TX                         : out   std_logic;                                        -- hps_io_uart0_inst_TX
			hps_hps_io_i2c0_inst_SDA                         : inout std_logic                     := 'X';             -- hps_io_i2c0_inst_SDA
			hps_hps_io_i2c0_inst_SCL                         : inout std_logic                     := 'X';             -- hps_io_i2c0_inst_SCL
			hps_hps_io_i2c1_inst_SDA                         : inout std_logic                     := 'X';             -- hps_io_i2c1_inst_SDA
			hps_hps_io_i2c1_inst_SCL                         : inout std_logic                     := 'X';             -- hps_io_i2c1_inst_SCL
			hps_hps_io_gpio_inst_GPIO09                      : inout std_logic                     := 'X';             -- hps_io_gpio_inst_GPIO09
			hps_hps_io_gpio_inst_GPIO35                      : inout std_logic                     := 'X';             -- hps_io_gpio_inst_GPIO35
			hps_hps_io_gpio_inst_GPIO40                      : inout std_logic                     := 'X';             -- hps_io_gpio_inst_GPIO40
			hps_hps_io_gpio_inst_GPIO48                      : inout std_logic                     := 'X';             -- hps_io_gpio_inst_GPIO48
			hps_hps_io_gpio_inst_GPIO53                      : inout std_logic                     := 'X';             -- hps_io_gpio_inst_GPIO53
			hps_hps_io_gpio_inst_GPIO54                      : inout std_logic                     := 'X';             -- hps_io_gpio_inst_GPIO54
			hps_hps_io_gpio_inst_GPIO61                      : inout std_logic                     := 'X';             -- hps_io_gpio_inst_GPIO61
			hps_ddr3_mem_a                                   : out   std_logic_vector(14 downto 0);                    -- mem_a
			hps_ddr3_mem_ba                                  : out   std_logic_vector(2 downto 0);                     -- mem_ba
			hps_ddr3_mem_ck                                  : out   std_logic;                                        -- mem_ck
			hps_ddr3_mem_ck_n                                : out   std_logic;                                        -- mem_ck_n
			hps_ddr3_mem_cke                                 : out   std_logic;                                        -- mem_cke
			hps_ddr3_mem_cs_n                                : out   std_logic;                                        -- mem_cs_n
			hps_ddr3_mem_ras_n                               : out   std_logic;                                        -- mem_ras_n
			hps_ddr3_mem_cas_n                               : out   std_logic;                                        -- mem_cas_n
			hps_ddr3_mem_we_n                                : out   std_logic;                                        -- mem_we_n
			hps_ddr3_mem_reset_n                             : out   std_logic;                                        -- mem_reset_n
			hps_ddr3_mem_dq                                  : inout std_logic_vector(31 downto 0) := (others => 'X'); -- mem_dq
			hps_ddr3_mem_dqs                                 : inout std_logic_vector(3 downto 0)  := (others => 'X'); -- mem_dqs
			hps_ddr3_mem_dqs_n                               : inout std_logic_vector(3 downto 0)  := (others => 'X'); -- mem_dqs_n
			hps_ddr3_mem_odt                                 : out   std_logic;                                        -- mem_odt
			hps_ddr3_mem_dm                                  : out   std_logic_vector(3 downto 0);                     -- mem_dm
			hps_ddr3_oct_rzqin                               : in    std_logic                     := 'X';             -- oct_rzqin
			reset_reset_n                                    : in    std_logic                     := 'X'              -- reset_n
		);
	end component soc_system;

	u0 : component soc_system
		port map (
			audio_0_external_interface_BCLK                  => CONNECTED_TO_audio_0_external_interface_BCLK,                  --                  audio_0_external_interface.BCLK
			audio_0_external_interface_DACDAT                => CONNECTED_TO_audio_0_external_interface_DACDAT,                --                                            .DACDAT
			audio_0_external_interface_DACLRCK               => CONNECTED_TO_audio_0_external_interface_DACLRCK,               --                                            .DACLRCK
			audio_and_video_config_0_external_interface_SDAT => CONNECTED_TO_audio_and_video_config_0_external_interface_SDAT, -- audio_and_video_config_0_external_interface.SDAT
			audio_and_video_config_0_external_interface_SCLK => CONNECTED_TO_audio_and_video_config_0_external_interface_SCLK, --                                            .SCLK
			audio_pll_0_audio_clk_clk                        => CONNECTED_TO_audio_pll_0_audio_clk_clk,                        --                       audio_pll_0_audio_clk.clk
			clk_clk                                          => CONNECTED_TO_clk_clk,                                          --                                         clk.clk
			hps_hps_io_emac1_inst_TX_CLK                     => CONNECTED_TO_hps_hps_io_emac1_inst_TX_CLK,                     --                                         hps.hps_io_emac1_inst_TX_CLK
			hps_hps_io_emac1_inst_TXD0                       => CONNECTED_TO_hps_hps_io_emac1_inst_TXD0,                       --                                            .hps_io_emac1_inst_TXD0
			hps_hps_io_emac1_inst_TXD1                       => CONNECTED_TO_hps_hps_io_emac1_inst_TXD1,                       --                                            .hps_io_emac1_inst_TXD1
			hps_hps_io_emac1_inst_TXD2                       => CONNECTED_TO_hps_hps_io_emac1_inst_TXD2,                       --                                            .hps_io_emac1_inst_TXD2
			hps_hps_io_emac1_inst_TXD3                       => CONNECTED_TO_hps_hps_io_emac1_inst_TXD3,                       --                                            .hps_io_emac1_inst_TXD3
			hps_hps_io_emac1_inst_RXD0                       => CONNECTED_TO_hps_hps_io_emac1_inst_RXD0,                       --                                            .hps_io_emac1_inst_RXD0
			hps_hps_io_emac1_inst_MDIO                       => CONNECTED_TO_hps_hps_io_emac1_inst_MDIO,                       --                                            .hps_io_emac1_inst_MDIO
			hps_hps_io_emac1_inst_MDC                        => CONNECTED_TO_hps_hps_io_emac1_inst_MDC,                        --                                            .hps_io_emac1_inst_MDC
			hps_hps_io_emac1_inst_RX_CTL                     => CONNECTED_TO_hps_hps_io_emac1_inst_RX_CTL,                     --                                            .hps_io_emac1_inst_RX_CTL
			hps_hps_io_emac1_inst_TX_CTL                     => CONNECTED_TO_hps_hps_io_emac1_inst_TX_CTL,                     --                                            .hps_io_emac1_inst_TX_CTL
			hps_hps_io_emac1_inst_RX_CLK                     => CONNECTED_TO_hps_hps_io_emac1_inst_RX_CLK,                     --                                            .hps_io_emac1_inst_RX_CLK
			hps_hps_io_emac1_inst_RXD1                       => CONNECTED_TO_hps_hps_io_emac1_inst_RXD1,                       --                                            .hps_io_emac1_inst_RXD1
			hps_hps_io_emac1_inst_RXD2                       => CONNECTED_TO_hps_hps_io_emac1_inst_RXD2,                       --                                            .hps_io_emac1_inst_RXD2
			hps_hps_io_emac1_inst_RXD3                       => CONNECTED_TO_hps_hps_io_emac1_inst_RXD3,                       --                                            .hps_io_emac1_inst_RXD3
			hps_hps_io_sdio_inst_CMD                         => CONNECTED_TO_hps_hps_io_sdio_inst_CMD,                         --                                            .hps_io_sdio_inst_CMD
			hps_hps_io_sdio_inst_D0                          => CONNECTED_TO_hps_hps_io_sdio_inst_D0,                          --                                            .hps_io_sdio_inst_D0
			hps_hps_io_sdio_inst_D1                          => CONNECTED_TO_hps_hps_io_sdio_inst_D1,                          --                                            .hps_io_sdio_inst_D1
			hps_hps_io_sdio_inst_CLK                         => CONNECTED_TO_hps_hps_io_sdio_inst_CLK,                         --                                            .hps_io_sdio_inst_CLK
			hps_hps_io_sdio_inst_D2                          => CONNECTED_TO_hps_hps_io_sdio_inst_D2,                          --                                            .hps_io_sdio_inst_D2
			hps_hps_io_sdio_inst_D3                          => CONNECTED_TO_hps_hps_io_sdio_inst_D3,                          --                                            .hps_io_sdio_inst_D3
			hps_hps_io_usb1_inst_D0                          => CONNECTED_TO_hps_hps_io_usb1_inst_D0,                          --                                            .hps_io_usb1_inst_D0
			hps_hps_io_usb1_inst_D1                          => CONNECTED_TO_hps_hps_io_usb1_inst_D1,                          --                                            .hps_io_usb1_inst_D1
			hps_hps_io_usb1_inst_D2                          => CONNECTED_TO_hps_hps_io_usb1_inst_D2,                          --                                            .hps_io_usb1_inst_D2
			hps_hps_io_usb1_inst_D3                          => CONNECTED_TO_hps_hps_io_usb1_inst_D3,                          --                                            .hps_io_usb1_inst_D3
			hps_hps_io_usb1_inst_D4                          => CONNECTED_TO_hps_hps_io_usb1_inst_D4,                          --                                            .hps_io_usb1_inst_D4
			hps_hps_io_usb1_inst_D5                          => CONNECTED_TO_hps_hps_io_usb1_inst_D5,                          --                                            .hps_io_usb1_inst_D5
			hps_hps_io_usb1_inst_D6                          => CONNECTED_TO_hps_hps_io_usb1_inst_D6,                          --                                            .hps_io_usb1_inst_D6
			hps_hps_io_usb1_inst_D7                          => CONNECTED_TO_hps_hps_io_usb1_inst_D7,                          --                                            .hps_io_usb1_inst_D7
			hps_hps_io_usb1_inst_CLK                         => CONNECTED_TO_hps_hps_io_usb1_inst_CLK,                         --                                            .hps_io_usb1_inst_CLK
			hps_hps_io_usb1_inst_STP                         => CONNECTED_TO_hps_hps_io_usb1_inst_STP,                         --                                            .hps_io_usb1_inst_STP
			hps_hps_io_usb1_inst_DIR                         => CONNECTED_TO_hps_hps_io_usb1_inst_DIR,                         --                                            .hps_io_usb1_inst_DIR
			hps_hps_io_usb1_inst_NXT                         => CONNECTED_TO_hps_hps_io_usb1_inst_NXT,                         --                                            .hps_io_usb1_inst_NXT
			hps_hps_io_spim1_inst_CLK                        => CONNECTED_TO_hps_hps_io_spim1_inst_CLK,                        --                                            .hps_io_spim1_inst_CLK
			hps_hps_io_spim1_inst_MOSI                       => CONNECTED_TO_hps_hps_io_spim1_inst_MOSI,                       --                                            .hps_io_spim1_inst_MOSI
			hps_hps_io_spim1_inst_MISO                       => CONNECTED_TO_hps_hps_io_spim1_inst_MISO,                       --                                            .hps_io_spim1_inst_MISO
			hps_hps_io_spim1_inst_SS0                        => CONNECTED_TO_hps_hps_io_spim1_inst_SS0,                        --                                            .hps_io_spim1_inst_SS0
			hps_hps_io_uart0_inst_RX                         => CONNECTED_TO_hps_hps_io_uart0_inst_RX,                         --                                            .hps_io_uart0_inst_RX
			hps_hps_io_uart0_inst_TX                         => CONNECTED_TO_hps_hps_io_uart0_inst_TX,                         --                                            .hps_io_uart0_inst_TX
			hps_hps_io_i2c0_inst_SDA                         => CONNECTED_TO_hps_hps_io_i2c0_inst_SDA,                         --                                            .hps_io_i2c0_inst_SDA
			hps_hps_io_i2c0_inst_SCL                         => CONNECTED_TO_hps_hps_io_i2c0_inst_SCL,                         --                                            .hps_io_i2c0_inst_SCL
			hps_hps_io_i2c1_inst_SDA                         => CONNECTED_TO_hps_hps_io_i2c1_inst_SDA,                         --                                            .hps_io_i2c1_inst_SDA
			hps_hps_io_i2c1_inst_SCL                         => CONNECTED_TO_hps_hps_io_i2c1_inst_SCL,                         --                                            .hps_io_i2c1_inst_SCL
			hps_hps_io_gpio_inst_GPIO09                      => CONNECTED_TO_hps_hps_io_gpio_inst_GPIO09,                      --                                            .hps_io_gpio_inst_GPIO09
			hps_hps_io_gpio_inst_GPIO35                      => CONNECTED_TO_hps_hps_io_gpio_inst_GPIO35,                      --                                            .hps_io_gpio_inst_GPIO35
			hps_hps_io_gpio_inst_GPIO40                      => CONNECTED_TO_hps_hps_io_gpio_inst_GPIO40,                      --                                            .hps_io_gpio_inst_GPIO40
			hps_hps_io_gpio_inst_GPIO48                      => CONNECTED_TO_hps_hps_io_gpio_inst_GPIO48,                      --                                            .hps_io_gpio_inst_GPIO48
			hps_hps_io_gpio_inst_GPIO53                      => CONNECTED_TO_hps_hps_io_gpio_inst_GPIO53,                      --                                            .hps_io_gpio_inst_GPIO53
			hps_hps_io_gpio_inst_GPIO54                      => CONNECTED_TO_hps_hps_io_gpio_inst_GPIO54,                      --                                            .hps_io_gpio_inst_GPIO54
			hps_hps_io_gpio_inst_GPIO61                      => CONNECTED_TO_hps_hps_io_gpio_inst_GPIO61,                      --                                            .hps_io_gpio_inst_GPIO61
			hps_ddr3_mem_a                                   => CONNECTED_TO_hps_ddr3_mem_a,                                   --                                    hps_ddr3.mem_a
			hps_ddr3_mem_ba                                  => CONNECTED_TO_hps_ddr3_mem_ba,                                  --                                            .mem_ba
			hps_ddr3_mem_ck                                  => CONNECTED_TO_hps_ddr3_mem_ck,                                  --                                            .mem_ck
			hps_ddr3_mem_ck_n                                => CONNECTED_TO_hps_ddr3_mem_ck_n,                                --                                            .mem_ck_n
			hps_ddr3_mem_cke                                 => CONNECTED_TO_hps_ddr3_mem_cke,                                 --                                            .mem_cke
			hps_ddr3_mem_cs_n                                => CONNECTED_TO_hps_ddr3_mem_cs_n,                                --                                            .mem_cs_n
			hps_ddr3_mem_ras_n                               => CONNECTED_TO_hps_ddr3_mem_ras_n,                               --                                            .mem_ras_n
			hps_ddr3_mem_cas_n                               => CONNECTED_TO_hps_ddr3_mem_cas_n,                               --                                            .mem_cas_n
			hps_ddr3_mem_we_n                                => CONNECTED_TO_hps_ddr3_mem_we_n,                                --                                            .mem_we_n
			hps_ddr3_mem_reset_n                             => CONNECTED_TO_hps_ddr3_mem_reset_n,                             --                                            .mem_reset_n
			hps_ddr3_mem_dq                                  => CONNECTED_TO_hps_ddr3_mem_dq,                                  --                                            .mem_dq
			hps_ddr3_mem_dqs                                 => CONNECTED_TO_hps_ddr3_mem_dqs,                                 --                                            .mem_dqs
			hps_ddr3_mem_dqs_n                               => CONNECTED_TO_hps_ddr3_mem_dqs_n,                               --                                            .mem_dqs_n
			hps_ddr3_mem_odt                                 => CONNECTED_TO_hps_ddr3_mem_odt,                                 --                                            .mem_odt
			hps_ddr3_mem_dm                                  => CONNECTED_TO_hps_ddr3_mem_dm,                                  --                                            .mem_dm
			hps_ddr3_oct_rzqin                               => CONNECTED_TO_hps_ddr3_oct_rzqin,                               --                                            .oct_rzqin
			reset_reset_n                                    => CONNECTED_TO_reset_reset_n                                     --                                       reset.reset_n
		);

