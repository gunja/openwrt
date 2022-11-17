module("luci.controller.lorawan", package.seeall)

function index()
	entry( {"admin", "lorawan"}, template("lorawan"), _("LoraWan"), 99 )
	entry( {"admin", "lorawan", "get_data"}, call("get_data")).leaf = true
	entry( {"admin", "lorawan", "set_data"}, post("set_data")).leaf = true
	entry( {"admin", "lorawan", "restart_lora"}, call("restart_lora")).leaf = true
	entry( {"admin", "lorawan", "upload"},   call("lora_cfg_upload"))
end

require "nixio.fs"
require "luci.jsonc"

 dir = "/etc"

function get_data()
	local cfg, json_cfg
	local json_out = { }
	cfg = nixio.fs.readfile(dir .. "/global_conf.json", 524288)
	parser = luci.jsonc.new()
	json_cfg = luci.jsonc.parse(cfg)

	json_out["gateway-id"] = json_cfg.gateway_conf.gateway_ID
	json_out["server-address"] = json_cfg.gateway_conf.server_address
	json_out["serv-port-up"] = json_cfg.gateway_conf.serv_port_up
	json_out["serv-port-down"] = json_cfg.gateway_conf.serv_port_down

	luci.http.prepare_content("application/json")
	luci.http.write_json(json_out or {})

end

function set_data()
	local cfg, json_cfg
	local json_str

	cfg = nixio.fs.readfile(dir .. "/global_conf.json", 524288)
	parser = luci.jsonc.new()
	json_cfg = luci.jsonc.parse(cfg)

	local gw_id = luci.http.formvalue("gateway-id");
	local srv_addr = luci.http.formvalue("server-address")
	local srv_pup = luci.http.formvalue("serv-port-up")
	local srv_pdn = luci.http.formvalue("serv-port-down")

	if gw_id ~= nil and gw_id.len ~= nil then
		json_cfg.gateway_conf.gateway_ID = gw_id
	end
	if srv_addr ~= nil and srv_addr.len ~= nil then
		json_cfg.gateway_conf.server_address = srv_addr
	end
	if srv_pup ~= nil and srv_pup.len ~= nil then
		json_cfg.gateway_conf.serv_port_up = tonumber(srv_pup)
	end
	if srv_pdn ~= nil and srv_pdn.len ~= nil then
		json_cfg.gateway_conf.serv_port_down = tonumber(srv_pdn)
	end

	json_str = luci.jsonc.stringify(json_cfg, true)
	nixio.fs.writefile(dir .. "/global_conf.json", json_str)

	luci.http.prepare_content("text/plain; charset=utf-8")
	luci.http.write("ok");
end

function restart_lora()
	luci.sys.exec("/etc/init.d/lora_pkt_fwd restart")

	luci.http.prepare_content("text/plain; charset=utf-8")
	luci.http.write_json("ok");
end

function lora_cfg_upload()
	local fs      = require("nixio.fs")
	local http    = require("luci.http")
	local util    = require("luci.util")
	local uci     = require("luci.model.uci").cursor()
	local upload  = http.formvalue("pf_config")
	local name    = http.formvalue("instance_name2")
	local basedir = "/etc"
	local file    = basedir.. "/global_conf.json"

	if not fs.stat(basedir) then
		fs.mkdir(basedir)
	end

	if upload then
		local fp

		http.setfilehandler(
			function(meta, chunk, eof)
				local data = util.trim(chunk:gsub("\r\n", "\n")) .. "\n"
				data = util.trim(data:gsub("[\128-\255]", ""))

				if not fp and meta and meta.name == "pf_config" then
					fp = io.open(file, "w")
				end
				if fp and data then
					fp:write(data)
				end
				if fp and eof then
					fp:close()
					luci.sys.exec("/etc/init.d/lora_pkt_fwd restart")
				end
			end
		)
	end
	http.redirect(luci.dispatcher.build_url('admin/lorawan/'))
end
