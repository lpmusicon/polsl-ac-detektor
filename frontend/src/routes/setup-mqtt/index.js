import { Field, Formik } from "formik";

import { route } from "preact-router";

import { useEffect, useState } from "preact/hooks";

import { useFetch, usePost } from "../../api";
import {
  MQTT_STATUS,
  processRedirect,
  transformRequest,
  logError,
} from "../../utils";
import { Routes } from "../../components/app.routes";
import { BatteryStatus } from "./../../components/battery.status";
import { Button } from "../../components/button";
import { FieldWrapper } from "../../components/field.wrapper";
import { FillContainer } from "../../components/fill.container";
import { Input } from "../../components/input";
import { Statusbar } from "../../components/statusbar";
import { Typography, Variant } from "../../components/typography";

const initialValues = {
  server: "",
  user: "",
  password: "",
  port: "1883",
};

export const SetupMQTT = () => {
  const { fetch: setupState } = useFetch("/api/setup");
  const { post: connectMQTT } = usePost("/api/mqtt/connect");
  const { fetch: statusMQTT } = useFetch("/api/mqtt/status");
  const [error, setError] = useState();

  useEffect(() => {
    const setup = async () => {
      try {
        const { status } = await setupState();
        processRedirect(status);
      } catch (e) {
        logError(e);
      }
    };
    setup();
  }, []);

  const onSubmit = async (values) => {
    try {
      await connectMQTT(transformRequest(values));
      const interval = setInterval(async () => {
        const { status } = await statusMQTT();
        clearInterval(interval);
        switch (status) {
          case MQTT_STATUS.DISCONNECTED:
            setError(
              "Nie udało się podłączyć do serwera MQTT. Spróbuj jeszcze raz"
            );
            break;
          case MQTT_STATUS.CONNECTED:
            route(Routes.Dashboard.path, true);
            break;
          default:
            break;
        }
      }, 1000);
    } catch (e) {
      logError(e);
    }
  };

  return (
    <Formik initialValues={initialValues} onSubmit={onSubmit}>
      {(props) => (
        <>
          <Statusbar left={<BatteryStatus />} />
          <FillContainer>
            <Typography variant={Variant.h1} text="Witaj" />
            <Typography
              variant={Variant.body1}
              text="Proszę skonfigurować ustawienia serwera MQTT"
            />
            <Typography
              variant={Variant.h2}
              withMargin
              text="Podaj wymagane pola"
            />
            <FieldWrapper first>
              <Field
                component={Input}
                label="Serwer"
                name="server"
                type="text"
                autocomplete="off"
                placeholder="mqtt.example.com"
              />
            </FieldWrapper>
            <FieldWrapper>
              <Field
                component={Input}
                label="Użytkownik"
                name="user"
                type="text"
                autocomplete="off"
                placeholder="user.mqtt"
              />
            </FieldWrapper>
            <FieldWrapper>
              <Field
                component={Input}
                label="Hasło"
                name="password"
                type="password"
                autocomplete="off"
                placeholder="********"
              />
            </FieldWrapper>
            <FieldWrapper last>
              <Field
                component={Input}
                label="Port"
                name="port"
                type="text"
                autocomplete="off"
                placeholder="3306"
              />
            </FieldWrapper>
            {error && (
              <Typography variant={Variant.accent} withMargin text={error} />
            )}
          </FillContainer>
          <Button type="submit" onClick={() => props.handleSubmit()}>
            Sprawdź ustawienia
          </Button>
        </>
      )}
    </Formik>
  );
};
