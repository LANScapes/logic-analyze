/*
 * Isolated Python start-up for the macOS app bundle.
 *
 * Built against the full Python API (PyConfig is not part of the limited API
 * the rest of libsigrokdecode uses). The bundle ships its own Python, so the
 * interpreter ignores PYTHON* variables, the user site, virtual-environment
 * hooks and the current directory, skips site import, and never writes
 * bytecode into the read-only bundle. Signal handlers stay with the host.
 *
 * This file is part of the libsigrokdecode project (GPL-3.0-or-later).
 */
#ifdef __APPLE__
#include <Python.h>
#include <stdlib.h>

int srd_python_init_isolated(char *err, size_t errlen)
{
	PyConfig config;
	PyStatus status;

	/* Isolated mode still honours the macOS venv launcher hook; clear it here
	 * too so the library is safe even if the host did not. */
	unsetenv("__PYVENV_LAUNCHER__");
	PyConfig_InitIsolatedConfig(&config);
	config.install_signal_handlers = 0;
	config.site_import = 0;
	config.write_bytecode = 0;
	status = Py_InitializeFromConfig(&config);
	PyConfig_Clear(&config);
	if (PyStatus_Exception(status)) {
		if (err && errlen)
			snprintf(err, errlen, "%s", status.err_msg ? status.err_msg : "unknown error");
		return -1;
	}
	return 0;
}
#endif /* __APPLE__ */
