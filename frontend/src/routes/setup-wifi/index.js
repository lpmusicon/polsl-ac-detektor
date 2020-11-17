import { Typography, Variant } from "../../components/typography";
import { WIFI_STATUS, processRedirect } from "./../../utils";
import { WifiBar, WifiEntry, WifiEntryContainer } from "./components";
import { useEffect, useState } from "preact/hooks";
import { useFetch, usePost } from "../../api";

import { BatteryStatus } from "./../../components/battery.status";
import { Button } from "../../components/button";
import { FillContainer } from "../../components/fill.container";
import { Formik } from "formik";
import { Routes } from "../../components/app.routes";
import { Statusbar } from "../../components/statusbar";
import { route } from "preact-router";

export const SetupWifi = () => {
  const { fetch: setupState } = useFetch("/api/setup");
  const { post: scanWifi } = usePost("/api/wifi/scan");
  const { fetch: fetchNetworks } = useFetch("/api/wifi/networks");
  const { post: connect } = usePost("/api/wifi/connect");
  const { fetch: statusWifi } = useFetch("/api/wifi/status");
  const [networks, setNetworks] = useState([]);
  const [selected, setSelected] = useState(-1);

  const refreshNetworks = async () => {
    const { networks } = await fetchNetworks();
    for (let i = 0; i < networks.length; i++) {
      networks[i].id = i;
    }
    setNetworks(networks);
  };

  useEffect(() => {
    const setup = async () => {
      const { status } = await setupState();
      processRedirect(status);
      await scanWifi();

      setTimeout(async () => {
        try {
          await refreshNetworks();
        } catch (e) {
          console.warn("Error: ", e);
        }
      }, 1000);
    };
    setup();
  }, []);

  const onSubmit = async (values) => {
    await connect(values);
    const interval = setInterval(async () => {
      const { status } = await statusWifi();
      if (status === WIFI_STATUS.WL_CONNECTED) {
        clearInterval(interval);
        route(Routes.SetupMQTT.path, true);
      }
    }, 1000);
  };

  return (
    <Formik initialValues={{ password: "" }} onSubmit={onSubmit}>
      {(props) => (
        <>
          <Statusbar left={<BatteryStatus />} />
          <FillContainer>
            <Typography variant={Variant.h1} text="Witaj" />
            <Typography
              variant={Variant.body1}
              text="Pora skonfigurować twoje urządzenie"
            />
            <Typography
              variant={Variant.h2}
              text="Zacznijmy od podłączenia do sieci WiFi"
            />
            <WifiBar onRefresh={refreshNetworks} />
            <WifiEntryContainer>
              {networks.map((network) => (
                <WifiEntry
                  setActive={setSelected}
                  network={network}
                  selected={selected === network.id}
                />
              ))}
            </WifiEntryContainer>
          </FillContainer>
          <Button type="submit" onClick={() => props.handleSubmit()}>
            Sprawdź ustawienia
          </Button>
        </>
      )}
    </Formik>
  );
};
