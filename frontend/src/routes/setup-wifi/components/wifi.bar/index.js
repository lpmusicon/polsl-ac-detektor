import { Typography, Variant } from "../../../../components/typography";

import styled from "styled-components";

const BarWrapper = styled.div`
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  align-items: center;
`;

const SmallIcon = styled.img`
  display: block;
  width: 24px;
  height: 24px;
  cursor: pointer;
`;

const WifiBar = ({ onRefresh = () => {} }) => {
  return (
    <BarWrapper>
      <Typography variant={Variant.body2} text="Lista sieci:" withMargin />
      <SmallIcon src="/assets/refresh.svg" onClick={onRefresh} />
    </BarWrapper>
  );
};

export default WifiBar;
