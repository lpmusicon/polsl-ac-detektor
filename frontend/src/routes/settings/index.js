import { Field, Formik } from "formik";
import { Typography, Variant } from "../../components/typography";
import { useEffect, useState } from "preact/hooks";

import { BatteryStatus } from "./../../components/battery.status";
import { FieldWrapper } from "../../components/field.wrapper";
import { FillContainer } from "../../components/fill.container";
import { IconLink } from "../../components/icon.link";
import { Input } from "../../components/input";
import { Routes } from "../../components/app.routes";
import { Statusbar } from "../../components/statusbar";
import { useFetch } from "../../api";

export const Settings = () => {
  const { fetch: getSettings } = useFetch("/api/setup/config");
  const [initialValues, setInitialValues] = useState({});
  useEffect(() => {
    const setup = async () => {
      const values = await getSettings();
      setInitialValues(values);
    };
    setup();
  }, []);

  return (
    <Formik
      enableReinitialize
      initialValues={initialValues}
      onSubmit={() => {}}
    >
      {(props) => (
        <>
          <Statusbar
            left={<BatteryStatus />}
            title="Ustawienia"
            right={
              <IconLink
                src="/assets/close.svg"
                alt="zamknij"
                href={Routes.Dashboard.path}
              />
            }
          />
          <FillContainer>
            <Typography variant={Variant.h1} withMargin text="MQTT" />
            <FieldWrapper first>
              <Field
                disabled
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
                disabled
                component={Input}
                label="Użytkownik"
                name="user"
                type="text"
                autocomplete="off"
                placeholder="user.mqtt"
              />
            </FieldWrapper>
            <FieldWrapper last>
              <Field
                disabled
                component={Input}
                label="Port"
                name="port"
                type="text"
                autocomplete="off"
                placeholder="3306"
              />
            </FieldWrapper>
            <Typography variant={Variant.h1} withMargin text="WIFI" />
            <FieldWrapper first last only>
              <Field
                disabled
                component={Input}
                label="SSID"
                name="ssid"
                type="text"
                autocomplete="off"
                placeholder="mqtt.example.com"
              />
            </FieldWrapper>
            <Typography variant={Variant.h2} text="MAC Urządzenia" />
            <Typography
              variant={Variant.accent}
              text={initialValues?.mac || "---"}
            />
            <Typography variant={Variant.h2} text="Numer karty SIM" />
            <Typography
              variant={Variant.accent}
              text={initialValues?.phone || "---"}
            />
            <Typography variant={Variant.h2} text="Zasilanie" />
            <Typography
              variant={Variant.accent}
              text={initialValues?.ac || false ? "Podłączone" : "Odłączone"}
            />
            <Typography variant={Variant.h2} text="Bateria" />
            <Typography
              variant={Variant.accent}
              text={`Około ${initialValues?.battery || 3700}mV`}
            />
          </FillContainer>
        </>
      )}
    </Formik>
  );
};
