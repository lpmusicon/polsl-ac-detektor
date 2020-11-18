import { useEffect, useState } from "preact/hooks";

import { Formik } from "formik";

import { route } from "preact-router";

import { useFetch, usePost } from "../../api";
import {
  WIFI_STATUS,
  processRedirect,
  transformRequest,
  logError,
} from "./../../utils";
import { Routes } from "../../components/app.routes";
import { BatteryStatus } from "./../../components/battery.status";
import { Button } from "../../components/button";
import { FillContainer } from "../../components/fill.container";
import { Statusbar } from "../../components/statusbar";
import { Typography, Variant } from "../../components/typography";

import { WifiBar, WifiEntry, WifiEntryContainer } from "./components";

export const SetupWifi = () => {
  const { fetch: setupState } = useFetch("/api/setup");
  const { post: scanWifi } = usePost("/api/wifi/scan");
  const { post: rescanWifi } = usePost("/api/wifi/rescan");
  const { fetch: fetchNetworks } = useFetch("/api/wifi/networks");
  const { post: connect } = usePost("/api/wifi/connect");
  const { fetch: statusWifi } = useFetch("/api/wifi/status");
  const [networks, setNetworks] = useState([]);
  const [selected, setSelected] = useState(-1);
  const [lastStatus, setLastStatus] = useState();

  const refreshNetworks = async () => {
    const interval = setInterval(async () => {
      try {
        const { networks, statusCode: code } = await fetchNetworks();
        if (code === 200) {
          clearInterval(interval);
          for (let i = 0; i < networks.length; i++) {
            networks[i].id = i;
          }
          setNetworks(networks);
        }
      } catch (e) {
        logError(e);
      }
    }, 2000);
  };

  const rescanNetworks = async () => {
    try {
      setNetworks([]);
      await rescanWifi();
      refreshNetworks();
    } catch (e) {
      logError(e);
    }
  };

  useEffect(() => {
    const setup = async () => {
      try {
        const { status } = await setupState();
        processRedirect(status);
        await scanWifi();
      } catch (e) {
        logError(e);
      }
      refreshNetworks();
    };
    setup();
  }, []);

  const onSubmit = async (values) => {
    try {
      await connect(transformRequest(values));
      const interval = setInterval(async () => {
        try {
          const { status } = await statusWifi();
          clearInterval(interval);
          switch (status) {
            case WIFI_STATUS.WL_CONNECTED:
              route(Routes.SetupMQTT.path, true);
              break;
            case WIFI_STATUS.WL_CONNECT_FAILED:
            case WIFI_STATUS.WL_DISCONNECTED:
              setLastStatus("Nie udało się podłączyć do sieci");
              break;
            default:
              setLastStatus("Nieokreślony błąd, spróbuj ponownie");
              break;
          }
        } catch (e) {
          logError(e);
        }
      }, 2000);
    } catch (e) {
      logError(e);
    }
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
            {lastStatus && (
              <Typography variant={Variant.accent} text={lastStatus} />
            )}
            <WifiBar onRefresh={rescanNetworks} />
            <WifiEntryContainer>
              {networks.length === 0 && (
                <Typography variant={Variant.h2} text="Brak dostępnych sieci" />
              )}
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
