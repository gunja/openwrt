-- scp package/gs/luci-gs/luci/controller/gs.lua root@192.168.31.84:/usr/lib/lua/luci/controller/gs.lua
-- scp package/gs/luci-gs/luci/view/gs.htm root@192.168.31.84:/usr/lib/lua/luci/view/gs.htm
-- scp package/gs/gs/files/gpio_conf.json root@192.168.31.84:/etc/gpio_conf.json

module("luci.controller.gs", package.seeall)

function index()
	entry( {"admin", "gs"}, template("gs"), _("Gs"), 99)
	entry( {"admin", "gs", "configuration_get"}, call("gs_configuration_get")).leaf = true
	entry( {"admin", "gs", "configuration_set"}, post("gs_configuration_set")).leaf = true
	entry( {"admin", "gs", "value_get"}, call("gs_value_get")).leaf = true
	entry( {"admin", "gs", "value_set"}, post("gs_value_set")).leaf = true
	entry( {"admin", "gs", "state_get"}, call("gs_state_get")).leaf = true
end

require "nixio.fs"
require "luci.jsonc"

dir = "/etc"

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

function gs_state_get()
   	local cfg, json_cfg
	local json_out = { }
	local result = {}
	result.gpios = {}
	local gpios = {}
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

	luci.http.prepare_content("application/json")
-- 	luci.http.write_json(json_out or {})
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
	luci.sys.exec('echo "' .. value .. '" > /sys/class/gpio/gpio' .. gpio_number .. '/value')

	luci.http.prepare_content("text/plain; charset=utf-8")
	luci.http.write("ok");
end

-- function gs_configuration_set()
-- 	local cfg, json_cfg
-- 	local json_str
--
-- 	cfg = nixio.fs.readfile(dir .. "/gpio_conf.json", 524288)
-- 	parser = luci.jsonc.new()
-- 	json_cfg = luci.jsonc.parse(cfg)
--
-- 	-- parse gpios structure
-- 	gpios = luci.http.formvalue('gpios')
-- 	parser = luci.jsonc.new()
-- 	gpios_data = luci.jsonc.parse(gpios)
-- 	for i = 1, #gpios_data do
-- 		luci.sys.exec('echo "' .. gpios_data[i].controller_number .. '" >> /tmp/l')
-- 		luci.sys.exec('echo "' .. gpios_data[i].pad_number .. '" >> /tmp/l')
-- 		luci.sys.exec('echo "' .. gpios_data[i].direction .. '" >> /tmp/l')
--
-- 		local controller_number = gpios_data[i].controller_number
-- 		local pad_number = gpios_data[i].pad_number
-- 		local direction = gpios_data[i].direction
-- 		local gpio_number = (controller_number - 1)*32 + pad_number
--
-- 		if nixio.fs.stat('/sys/class/gpio/gpio' .. gpio_number, 'type') ~= 'dir' then
-- 			luci.sys.exec('echo "' .. gpio_number .. '" > /sys/class/gpio/export')
-- 		end
-- 		luci.sys.exec('echo "' .. direction .. '" > /sys/class/gpio/gpio' .. gpio_number .. '/direction')
-- 	end
--
-- 	json_cfg.gpios = gpios_data
-- 	json_str = luci.jsonc.stringify(json_cfg, true)
-- 	nixio.fs.writefile(dir .. "/gpio_conf.json", json_str)
--
-- 	luci.http.prepare_content("text/plain; charset=utf-8")
-- 	luci.http.write("ok");

-- end

