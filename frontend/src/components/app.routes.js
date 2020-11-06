import { Dashboard } from "../routes/dashboard";
import { Settings } from "../routes/settings";
import { SetupMQTT } from "../routes/setup-mqtt";
import { SetupWifi } from "../routes/setup-wifi";

export const Routes = {
  SetupWifi: { Component: SetupWifi, path: "/setup-wifi" },
  Settings: { Component: Settings, path: "/settings" },
  SetupMQTT: { Component: SetupMQTT, path: "/setup-mqtt" },
  Dashboard: { Component: Dashboard, path: "/" },
};
