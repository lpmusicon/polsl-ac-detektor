import styled from "styled-components";

import { Typography, Variant } from "../typography";

const Container = styled.div`
  padding: 12px;
  display: flex;
  align-items: center;
`;

const InputField = styled.input`
  flex: 1 0 auto;
  background: transparent;
  border: none;
  outline: none;
  color: #44d194;
  font-size: 18px;
  font-family: "Montserrat", Arial, Helvetica, sans-serif;
  margin: 0 24px 0 12px;
`;

export const Input = ({ field, label, ...props }) => {
  return (
    <Container>
      <Typography variant={Variant.body2} text={label} />
      <InputField {...props} {...field} />
    </Container>
  );
};
