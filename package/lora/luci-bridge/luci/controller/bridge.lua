module("luci.controller.bridge", package.seeall)

function index()
	entry( {"admin", "bridge"}, template("bridge"), _("ChirpStack"), 99 )
	entry( {"admin", "bridge", "upload"},   call("bridge_cfg_upload"))
end

require "nixio.fs"
require "luci.jsonc"

 dir = "/etc/chirpstack-gateway-bridge"

function bridge_cfg_upload()
	local fs      = require("nixio.fs")
	local http    = require("luci.http")
	local util    = require("luci.util")
	local uci     = require("luci.model.uci").cursor()
	local upload  = http.formvalue("pf_config")
	local name    = http.formvalue("instance_name2")
	local basedir = "/etc/chirpstack-gateway-bridge"
	local file    = basedir.. "/chirpstack-gateway-bridge.toml"

	if not fs.stat(basedir) then
		fs.mkdir(basedir)
	end

	if upload then
		local fp

		http.setfilehandler(
			function(meta, chunk, eof)
				local data = chunk

				if not fp and meta and meta.name == "pf_config" then
					fp = io.open(file, "w")
				end
				if fp and data then
					fp:write(data)
				end
				if fp and eof then
					fp:close()
					luci.sys.exec("/etc/init.d/cs-gateway restart")
				end
			end
		)
	end
	http.redirect(luci.dispatcher.build_url('admin/bridge/'))
end
