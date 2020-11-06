import { Field, Formik } from "formik";
import { MQTT_STATUS, processRedirect } from "../../utils";
import { Typography, Variant } from "../../components/typography";
import { useFetch, usePost } from "../../api";

import { BatteryStatus } from "./../../components/battery.status";
import { Button } from "../../components/button";
import { FieldWrapper } from "../../components/field.wrapper";
import { FillContainer } from "../../components/fill.container";
import { Input } from "../../components/input";
import { Routes } from "../../components/app.routes";
import { Statusbar } from "../../components/statusbar";
import { route } from "preact-router";
import { useEffect } from "preact/hooks";

const initialValues = {
  server: "",
  user: "",
  password: "",
  port: "3306",
};

export const SetupMQTT = () => {
  const { fetch: setupState } = useFetch("/api/setup");
  const { post: connectMQTT } = usePost("/api/mqtt/connect");
  const { fetch: statusMQTT } = useFetch("/api/mqtt/status");

  useEffect(() => {
    const setup = async () => {
      const { status } = await setupState();
      processRedirect(status);
    };
    setup();
  }, []);

  const onSubmit = async (values) => {
    try {
      await connectMQTT(values);
      const interval = setInterval(async () => {
        const { status } = await statusMQTT();
        switch (status) {
          case MQTT_STATUS.DISCONNECTED:
            clearInterval(interval);
            break;
          case MQTT_STATUS.CONNECTED:
            clearInterval(interval);
            route(Routes.Dashboard.path, true);
            break;
          case MQTT_STATUS.CONNECTING:
          default:
            break;
        }
      }, 1000);
    } catch (e) {
      console.warn("Error: ", e);
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
          </FillContainer>
          <Button type="submit" onClick={() => props.handleSubmit()}>
            Sprawdź ustawienia
          </Button>
        </>
      )}
    </Formik>
  );
};
