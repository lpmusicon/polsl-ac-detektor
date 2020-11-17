import { Typography, Variant } from "../../../../components/typography";

import { Input } from "./input";
import styled from "styled-components";

const PasswordStyled = styled.div`
  padding: 12px;
  display: flex;
  align-items: center;
  height: 60px;
`;

export const Password = ({ field, ...props }) => {
  return (
    <PasswordStyled>
      <Typography variant={Variant.body2} text="Hasło" />
      <Input {...props} {...field} />
    </PasswordStyled>
  );
};
