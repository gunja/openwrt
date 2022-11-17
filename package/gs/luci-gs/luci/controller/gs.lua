-- scp package/gs/luci-gs/luci/controller/gs.lua root@192.168.31.46:/usr/lib/lua/luci/controller/gs.lua
-- scp package/gs/luci-gs/luci/view/gs.htm root@192.168.31.46:/usr/lib/lua/luci/view/gs.htm
-- scp package/gs/luci-gs/styles/gs.css root@192.168.31.46:/www/luci-static/resources/gs.css
-- scp package/gs/gs/files/gpio_conf.json root@192.168.31.46:/etc/gpio_conf.json

module("luci.controller.gs", package.seeall)

function index()
	entry( {"admin", "gs"}, template("gs"), _("Gs"), 99)
	entry( {"admin", "gs", "configuration_get"}, call("gs_configuration_get")).leaf = true
	entry( {"admin", "gs", "configuration_set"}, post("gs_configuration_set")).leaf = true
	entry( {"admin", "gs", "settings_set"}, post("gs_settings_set")).leaf = true
	entry( {"admin", "gs", "value_get"}, call("gs_value_get")).leaf = true
	entry( {"admin", "gs", "value_set"}, post("gs_value_set")).leaf = true
	entry( {"admin", "gs", "state_get"}, call("gs_state_get")).leaf = true
	entry( {"admin", "gs", "firmware_version_get"}, call("gs_firmware_version_get")).leaf = true
end

require "nixio.fs"
require "luci.jsonc"

dir = "/etc"

function gs_firmware_version_get()
	local cfg, json_cfg
	local result = {}
	fw_ver = nixio.fs.readfile("/mnt/gs/1/firmware_version"):sub(1,-2)

	result = {
	        version = fw_ver,
	    },

	luci.http.prepare_content("application/json")
	luci.http.write_json(result or {})
end

function gs_configuration_get()
	local cfg, json_cfg
	local json_out = { }
	cfg = nixio.fs.readfile(dir .. "/gpio_conf.json", 524288)
	parser = luci.jsonc.new() 
	json_cfg = luci.jsonc.parse(cfg)

	json_out["gpios"] = json_cfg.gpios

	luci.http.prepare_content("application/json")
	luci.http.write_json(json_out or {})
end

function relay_code_to_gpio_number (relay_code)
    local gpio_number = '200'

    if relay_code == 'K1' then
        gpio_number = '5'
    elseif relay_code == 'K2' then
        gpio_number = '103'
    elseif relay_code == 'K3' then
        gpio_number = '104'
    elseif relay_code == 'K4' then
        gpio_number = '105'
    elseif relay_code == 'K5' then
        gpio_number = '110'
    elseif relay_code == 'K6' then
        gpio_number = '107'
    end

    return gpio_number
end

function gpio_number_to_relay_code (gpio_number)
    local relay_code = 'K0'

    if gpio_number == '5' then
        relay_code = 'K1'
    elseif gpio_number == '103' then
        relay_code = 'K2'
    elseif gpio_number == '104' then
        relay_code = 'K3'
    elseif gpio_number == '105' then
        relay_code = 'K4'
    elseif gpio_number == '110' then
        relay_code = 'K5'
    elseif gpio_number == '107' then
        relay_code = 'K6'
    end

    return relay_code
end

function gs_state_get()
   	local cfg, json_cfg
	local json_out = { }
	local result = {}
	result.gpios = {}
	result.telemetry = {}
	result.settings_left = {}
	result.settings_right = {}
	local gpios = {}
	local value_0 = tonumber(nixio.fs.readfile("/sys/bus/iio/devices/iio\:device0/in_voltage0_raw"):sub(1,-2))
	local value_1 = tonumber(nixio.fs.readfile("/sys/bus/iio/devices/iio\:device0/in_voltage1_raw"):sub(1,-2))
	local raw_value_0 = value_0
	local raw_value_1 = value_1
	if value_0 < 100 then
	    value_0 = 'broken'
	else
	    value_0 = string.format( "%.1f mA (%d)", (value_0 - 2100) / 537.5 + 4.0, value_0)
	end
	if value_1 < 100 then
	    value_1 = 'broken'
	else
	    value_1 = string.format( "%.1f mA (%d)", (value_1 - 2100) / 537.5 + 4.0, value_1)
	end

	local temperature_raw =  nixio.fs.readfile("/sys/class/hwmon/hwmon1/temp1_input"):sub(1,-2)
    local temperature = string.format( "%.1f °C", tonumber(temperature_raw) / 1000)
	result.telemetry = {
	    {
	        name = "temperature",
	        value = temperature,
	        raw_value = temperature_raw
	    },
	    {
	        name = "input_4_20_channel_0",
	        value = value_0,
	        raw_value = raw_value_0
	    },
	    {
	        name = "input_4_20_channel_1",
	        value = value_1,
	        raw_value = raw_value_1
	    }
	}

    local is_enabled = nixio.fs.readfile("/mnt/gs/1/isEnabled"):sub(1,-2)
    local density = nixio.fs.readfile("/mnt/gs/1/setting_gas_density"):sub(1,-2)
    local relay_cut_off = nixio.fs.readfile("/mnt/gs/1/setting_relay_cut_off_timing"):sub(1,-2)
    local mass_flow_threshold = nixio.fs.readfile("/mnt/gs/1/setting_mass_flow_rate_threshold_value"):sub(1,-2)
    local can_address = nixio.fs.readfile("/mnt/gs/1/can_deviceAddress"):sub(1,-2)
    local flomac_address = nixio.fs.readfile("/mnt/gs/1/modbus_address"):sub(1,-2)
    local gaskit_address = nixio.fs.readfile("/mnt/gs/1/address"):sub(1,-2)
    local relay_middle_gpio = nixio.fs.readfile("/mnt/gs/1/relay_middle_number"):sub(1,-2)
    local relay_high_gpio = nixio.fs.readfile("/mnt/gs/1/relay_high_number"):sub(1,-2)
    local is_pagz_mode_enabled = nixio.fs.readfile("/mnt/gs/1/isPAGZmodeEnabled"):sub(1,-2)
    local low_pressure_threshold = nixio.fs.readfile("/mnt/gs/1/setting_in_4_20_pressure_low_threshold"):sub(1,-2)
    local high_pressure_threshold = nixio.fs.readfile("/mnt/gs/1/setting_in_4_20_pressure_high_threshold"):sub(1,-2)
    local valves_amount = nixio.fs.readfile("/mnt/gs/1/setting_valves_amount"):sub(1,-2)
    local led_enabled = nixio.fs.readfile("/mnt/gs/1/led_enabled"):sub(1,-2)
    local modbus_port = nixio.fs.readfile("/mnt/gs/1/modbus_port"):sub(1,-2)

    local relay_middle_number = gpio_number_to_relay_code(relay_middle_gpio)
    local relay_high_number = gpio_number_to_relay_code(relay_high_gpio)

	result.settings_left = {
	    {
	        name = "enabled",
	        value = is_enabled,
	        label = "пост включен",
	        explanation = "вкл/выкл поста",
	        field_type = "dropdown",
	        field_options = { ["0"]='ВЫКЛ', ["1"]='ВКЛ' }
	    },
	    {
	        name = "gaskit address",
	        value = gaskit_address,
	        label = "адрес поста в ГазКите",
	        explanation = "нужно указать адрес, в качестве адреса Поста в ГазКите",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "display address",
	        value = can_address,
	        label = "адрес дисплея",
	        explanation = "если переключатель на дисплее выставлен в 0, то нужно 'ТУМБЛЕР 0' иначе 'ТУМБЛЕР 1'",
	        field_type = "dropdown",
	        field_options = { ["32"]='ТУМБЛЕР 0', ["64"]='ТУМБЛЕР 1' }
	    },
	    {
	        name = "flomac address",
	        value = flomac_address,
	        label = "адрес массомера",
	        explanation = "оставляйте значение по умолчанию 1, чтобы не возникало путаницы. Массомер каждого поста сидит на отдельном интерфейсе, а потому оба могут иметь один и тот же адрес",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "gas density",
	        value = density,
	        label = "плотность газа",
	        explanation = "значение плотности газа, полученное от Газпрома. Используется для преобразования показаний массомера в объем, запрошенный с ГазКита",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "relay cut-off",
	        value = relay_cut_off,
	        label = "отсечка клапана",
	        explanation = "калибровочный коэффициент упреждающего отключения клапана во избежание перелива",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "mass flow rate threshold",
	        value = mass_flow_threshold,
	        label = "расход газа при полном баке",
	        explanation = "уровень массового расхода газа при достижении которого Блок Управления поймет, что бак полон.",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "relay_middle_number",
	        value = relay_middle_number,
	        label = "клапан среднего давления",
	        explanation = "кодовое обозначение реле в распиновке Блока Управления, на которое назначен клапан среднего давления.",
	        field_type = "dropdown",
	        field_options = { ["K1"]='K1', ["K2"]='K2', ["K3"]='K3', ["K4"]='K4', ["K5"]='K5', ["K6"]='K6' }
	    },
	    {
	        name = "relay_high_number",
	        value = relay_high_number,
	        label = "клапан высокого давления",
	        explanation = "кодовое обозначение реле в распиновке Блока Управления, на которое назначен клапан высокого давления.",
	        field_type = "dropdown",
	        field_options = { ["K1"]='K1', ["K2"]='K2', ["K3"]='K3', ["K4"]='K4', ["K5"]='K5', ["K6"]='K6' }
	    },
	    {
	        name = "is_pagz_mode_enabled",
	        value = is_pagz_mode_enabled,
	        label = "режим ПАГЗ",
	        explanation = "1 - пост работает в режиме ПАГЗа (управление отпуском топлива по кнопке), 0 - режим ПАГЗ выключен (отпуск топлива с АРМ)",
	        field_type = "dropdown",
	        field_options = { ["0"]='ВЫКЛ', ["1"]='ВКЛ' }
	    },
	    {
	        name = "valves_amount",
	        value = valves_amount,
	        label = "количество клапанов",
	        explanation = "1 - используется только клапан среднего давления. 2 - используются клапаны среднего и высокого давления",
	        field_type = "dropdown",
	        field_options = { ["1"]='1', ["2"]='2' }
	    },
	    {
	        name = "low_pressure_threshold",
	        value = low_pressure_threshold,
	        label = "MIN давление (мА)",
	        explanation = "давление (мА) ниже данного значения рассматривается как ошибка",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "high_pressure_threshold",
	        value = high_pressure_threshold,
	        label = "MAX давление (мА)",
	        explanation = "давление (мА) выше данного значения рассматривается как ошибка",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "led_enabled",
	        value = led_enabled,
	        label = "лампы индикации",
	        explanation = "Лампы индикации режима работы/наличия ошибок",
	        field_type = "dropdown",
	        field_options = { ["0"]='ВЫКЛ', ["1"]='ВКЛ' }
	    },
	    {
	        name = "modbus_port",
	        value = modbus_port,
	        label = "разъем массомера",
	        explanation = "Разъем, к которому подключен массомер согласно маркировке на корпусе устройства.",
	        field_type = "dropdown",
	        field_options = { ["4"]='разъем А', ["5"]='разъем B' }
	    },
	}

    is_enabled = nixio.fs.readfile("/mnt/gs/2/isEnabled"):sub(1,-2)
    density = nixio.fs.readfile("/mnt/gs/2/setting_gas_density"):sub(1,-2)
    relay_cut_off = nixio.fs.readfile("/mnt/gs/2/setting_relay_cut_off_timing"):sub(1,-2)
    mass_flow_threshold = nixio.fs.readfile("/mnt/gs/2/setting_mass_flow_rate_threshold_value"):sub(1,-2)
    can_address = nixio.fs.readfile("/mnt/gs/2/can_deviceAddress"):sub(1,-2)
    flomac_address = nixio.fs.readfile("/mnt/gs/2/modbus_address"):sub(1,-2)
    gaskit_address = nixio.fs.readfile("/mnt/gs/2/address"):sub(1,-2)
    relay_middle_gpio = nixio.fs.readfile("/mnt/gs/2/relay_middle_number"):sub(1,-2)
    relay_high_gpio = nixio.fs.readfile("/mnt/gs/2/relay_high_number"):sub(1,-2)
    is_pagz_mode_enabled = nixio.fs.readfile("/mnt/gs/2/isPAGZmodeEnabled"):sub(1,-2)
    low_pressure_threshold = nixio.fs.readfile("/mnt/gs/2/setting_in_4_20_pressure_low_threshold"):sub(1,-2)
    high_pressure_threshold = nixio.fs.readfile("/mnt/gs/2/setting_in_4_20_pressure_high_threshold"):sub(1,-2)
    valves_amount = nixio.fs.readfile("/mnt/gs/2/setting_valves_amount"):sub(1,-2)
    led_enabled = nixio.fs.readfile("/mnt/gs/2/led_enabled"):sub(1,-2)
    modbus_port = nixio.fs.readfile("/mnt/gs/2/modbus_port"):sub(1,-2)

    relay_middle_number = gpio_number_to_relay_code(relay_middle_gpio)
    relay_high_number = gpio_number_to_relay_code(relay_high_gpio)

	result.settings_right = {
	    {
	        name = "enabled",
	        value = is_enabled,
	        label = "пост включен",
	        explanation = "вкл/выкл поста",
	        field_type = "dropdown",
	        field_options = { ["0"]='ВЫКЛ', ["1"]='ВКЛ' }
	    },
	    {
	        name = "gaskit address",
	        value = gaskit_address,
	        label = "адрес поста в ГазКите",
	        explanation = "нужно указать адрес, в качестве адреса Поста в ГазКите",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "display address",
	        value = can_address,
	        label = "адрес дисплея",
	        explanation = "если переключатель на дисплее выставлен в 0, то нужно 'ТУМБЛЕР 0' иначе 'ТУМБЛЕР 1'",
	        field_type = "dropdown",
	        field_options = { ["32"]='ТУМБЛЕР 0', ["64"]='ТУМБЛЕР 1' }
	    },
	    {
	        name = "flomac address",
	        value = flomac_address,
	        label = "адрес массомера",
	        explanation = "оставляйте значение по умолчанию 1, чтобы не возникало путаницы. Массомер каждого поста сидит на отдельном интерфейсе, а потому оба могут иметь один и тот же адрес",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "gas density",
	        value = density,
	        label = "плотность газа",
	        explanation = "значение плотности газа, полученное от Газпрома. Используется для преобразования показаний массомера в объем, запрошенный с ГазКита",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "relay cut-off",
	        value = relay_cut_off,
	        label = "отсечка клапана",
	        explanation = "калибровочный коэффициент упреждающего отключения клапана во избежание перелива",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "mass flow rate threshold",
	        value = mass_flow_threshold,
	        label = "расход газа при полном баке",
	        explanation = "уровень массового расхода газа при достижении которого Блок Управления поймет, что бак полон.",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "relay_middle_number",
	        value = relay_middle_number,
	        label = "клапан среднего давления",
	        explanation = "кодовое обозначение реле в распиновке Блока Управления, на которое назначен клапан среднего давления.",
	        field_type = "dropdown",
	        field_options = { ["K1"]='K1', ["K2"]='K2', ["K3"]='K3', ["K4"]='K4', ["K5"]='K5', ["K6"]='K6' }
	    },
	    {
	        name = "relay_high_number",
	        value = relay_high_number,
	        label = "клапан высокого давления",
	        explanation = "кодовое обозначение реле в распиновке Блока Управления, на которое назначен клапан высокого давления.",
	        field_type = "dropdown",
	        field_options = { ["K1"]='K1', ["K2"]='K2', ["K3"]='K3', ["K4"]='K4', ["K5"]='K5', ["K6"]='K6' }
	    },
	    {
	        name = "is_pagz_mode_enabled",
	        value = is_pagz_mode_enabled,
	        label = "режим ПАГЗ",
	        explanation = "1 - пост работает в режиме ПАГЗа (управление отпуском топлива по кнопке), 0 - режим ПАГЗ выключен (отпуск топлива с АРМ)",
	        field_type = "dropdown",
	        field_options = { ["0"]='ВЫКЛ', ["1"]='ВКЛ' }
	    },
	    {
	        name = "valves_amount",
	        value = valves_amount,
	        label = "количество клапанов",
	        explanation = "1 - используется только клапан среднего давления. 2 - используются клапаны среднего и высокого давления",
	        field_type = "dropdown",
	        field_options = { ["1"]='1', ["2"]='2' }
	    },
	    {
	        name = "low_pressure_threshold",
	        value = low_pressure_threshold,
	        label = "MIN давление (мА)",
	        explanation = "давление (мА) ниже данного значения рассматривается как ошибка",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "high_pressure_threshold",
	        value = high_pressure_threshold,
	        label = "MAX давление (мА)",
	        explanation = "давление (мА) выше данного значения рассматривается как ошибка",
	        field_type = "text",
	        field_options = {}
	    },
	    {
	        name = "led_enabled",
	        value = led_enabled,
	        label = "лампы индикации",
	        explanation = "Лампы индикации режима работы/наличия ошибок",
	        field_type = "dropdown",
	        field_options = { ["0"]='ВЫКЛ', ["1"]='ВКЛ' }
	    },
	    {
	        name = "modbus_port",
	        value = modbus_port,
	        label = "разъем массомера",
	        explanation = "Разъем, к которому подключен массомер согласно маркировке на корпусе устройства.",
	        field_type = "dropdown",
	        field_options = { ["4"]='разъем А', ["5"]='разъем B' }
	    }
	}

	local tmp = {}

	cfg = nixio.fs.readfile(dir .. "/gpio_conf.json", 524288)
	parser = luci.jsonc.new()
	json_cfg = luci.jsonc.parse(cfg)

    gpios = json_cfg.gpios
    for key, value in pairs(gpios) do
        tmp = value
	    local controller_number = tmp.controller_number
	    local pad_number = tmp.pad_number
        local gpio_number = (controller_number - 1)*32 + pad_number
        tmp.value = nixio.fs.readfile("/sys/class/gpio/gpio" .. gpio_number .. "/value"):sub(1,1)
        table.insert(result.gpios, tmp)
    end
	json_out["gpios"] = json_cfg.gpios
	table.insert(result, telemetry)
	table.insert(result, settings_left)
	table.insert(result, settings_right)

	luci.http.prepare_content("application/json")
    luci.http.write_json(result or {})
end

function gs_value_get()
	local controller_number = luci.http.formvalue('controller_number')
	local pad_number = luci.http.formvalue('pad_number')
	local gpio_number = (controller_number - 1)*32 + pad_number
	local value = 0
	if nixio.fs.stat('/sys/class/gpio/gpio' .. gpio_number .. '/value', 'type') == 'reg' then
		value = nixio.fs.readfile("/sys/class/gpio/gpio" .. gpio_number .. "/value"):sub(1,1)
		luci.sys.exec('echo "' .. gpio_number .. '" > /sys/class/gpio/export')
	end

	luci.http.prepare_content("application/json")
	local json_out = {}
	json_out.value = value
	luci.http.write_json(json_out)
end

function gs_value_set()
	-- parse gpios structure
	gpio = luci.http.formvalue('gpio')
	parser = luci.jsonc.new()
	gpio_data = luci.jsonc.parse(gpio)
	local controller_number = gpio_data.controller_number
	local pad_number = gpio_data.pad_number
	local value = gpio_data.value

	local gpio_number = (controller_number - 1)*32 + pad_number
	if nixio.fs.readfile('/sys/class/gpio/gpio' .. gpio_number .. '/direction'):sub(1,-2) == 'out' then
	    luci.sys.exec('echo "' .. value .. '" > /sys/class/gpio/gpio' .. gpio_number .. '/value')
    	luci.http.prepare_content("text/plain; charset=utf-8")
    	luci.http.write("ok");
    else
    	luci.http.prepare_content("text/plain; charset=utf-8")
	    luci.http.write("input");
    end
end

function gs_settings_set()
	local cfg, json_cfg
	local json_str

	-- parse gpios structure
	side = luci.http.formvalue('side')
	param = luci.http.formvalue('param')
	value = luci.http.formvalue('value')

    if side == 'settings_left' then
        side = '1'
    elseif side == 'settings_right' then
        side = '2'
    else
        luci.http.prepare_content("text/plain; charset=utf-8")
        luci.http.write('ERROR. unknown side: ' .. side);
    end

    if param == 'gas density' then
        param = 'setting_gas_density'
    elseif param == 'relay cut-off' then
        param = 'setting_relay_cut_off_timing'
    elseif param == 'mass flow rate threshold' then
        param = 'setting_mass_flow_rate_threshold_value'
    elseif param == 'enabled' then
        param = 'isEnabled'
    elseif param == 'flomac address' then
        param = 'modbus_address'
    elseif param == 'gaskit address' then
        param = 'address'
    elseif param == 'display address' then
        param = 'can_deviceAddress'
    elseif param == 'relay_middle_line' then
        param = 'relay_middle_number'
        value = relay_code_to_gpio_number(value)
    elseif param == 'relay_middle_number' then
        param = 'relay_middle_number'
        value = relay_code_to_gpio_number(value)
    elseif param == 'relay_high_number' then
        param = 'relay_high_number'
        value = relay_code_to_gpio_number(value)
    elseif param == 'is_pagz_mode_enabled' then
        param = 'isPAGZmodeEnabled'
    elseif param == 'valves_amount' then
        param = 'setting_valves_amount'
    elseif param == 'low_pressure_threshold' then
        param = 'setting_in_4_20_pressure_low_threshold'
    elseif param == 'high_pressure_threshold' then
        param = 'setting_in_4_20_pressure_high_threshold'
    elseif param == 'led_enabled' then
        param = 'led_enabled'
    elseif param == 'modbus_port' then
        param = 'modbus_port'
    else
        luci.http.prepare_content("text/plain; charset=utf-8")
        luci.http.write('ERROR. unknown param: ' .. param);
    end

    local file = '/mnt/gs/'.. side .. '/' .. param

    luci.sys.exec('echo ' .. value .. ' > ' .. file)

    local result = {
	    {
	        file = file,
	        side = side,
	        value = value,
	        result = 'ok'
	    }
    }

    luci.sys.exec('/etc/init.d/gs restart')

    luci.http.prepare_content("application/json")
    luci.http.write_json(result or {})
end


